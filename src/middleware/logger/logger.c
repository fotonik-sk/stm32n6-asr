/**
 * @file     logger.c
 * @brief    Log formatting and asynchronous transmission.
 * @details  Public contract is defined in logger.h. Internal functions
 *           use a shared mutex-protected buffer and avoid touching it
 *           while asynchronous output is in progress.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/
#include "logger.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32n6xx_hal.h"
#include "tx_api.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief Shared record-buffer size including the terminating NUL, in bytes. */
#define LOGGER_RECORD_SIZE 512U
/** @brief 100 ms rounded up to ticks; applied separately to mutex and output. */
#define LOGGER_WAIT_TICKS ((TX_TIMER_TICKS_PER_SECOND + 9UL) / 10UL)
/** @brief ANSI SGR(0), which restores the terminal's default attributes. */
#define ANSI_RESET "\033[0m"
/** @brief Marker that ends a truncated record body. */
#define LOGGER_ELLIPSIS "..."
/** @brief Length of LOGGER_ELLIPSIS without its NUL. */
#define LOGGER_ELLIPSIS_LENGTH (sizeof(LOGGER_ELLIPSIS) - 1U)

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Priority-inheriting mutex protecting the complete output cycle. */
static TX_MUTEX output_mutex;
/** @brief Mutex creation flag set before any consumer thread starts. */
static bool created;
/** @brief Atomic output callback accessed only while output_mutex is held. */
static logger_try_write_t output_try_write;
/** @brief Transfer-ready callback accessed only while output_mutex is held. */
static logger_output_ready_t output_ready;
/**
 * @brief Shared formatting buffer protected by output_mutex.
 * @details Static storage reduces caller stack usage. Before reuse, the logger
 * waits until the output releases its pointer to this buffer.
 */
static char record[LOGGER_RECORD_SIZE];

/** @brief Short labels ordered by logger_level_t. */
static const char *const level_names[] = {"CRIT", "ERROR", "WARN", "INFO"};
/** @brief ANSI color for each label, ordered by logger_level_t. */
static const char *const level_colors[] = {
    "\033[1;31m", "\033[31m", "\033[33m", "\033[32m"
};
/** @brief ANSI sequences ordered by logger_color_t; DEFAULT is an empty string. */
static const char *const text_colors[] = {
    "", "\033[31m", "\033[32m", "\033[33m", "\033[34m",
    "\033[35m", "\033[36m", "\033[37m"
};

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Check that the call runs in a thread with interrupts enabled.
 * @return true for a valid context; false for an ISR or forbidden wait context.
 * @note The current thread may remain set inside an ISR, so IPSR is checked too.
 */
static bool is_thread_context(void)
{
    return (__get_IPSR() == 0U) && (__get_PRIMASK() == 0U) &&
           (__get_BASEPRI() == 0U) && (tx_thread_identify() != TX_NULL);
}

/**
 * @brief Validate the calling context and acquire the logger mutex.
 * @retval LOGGER_OK Mutex acquired; the caller must call unlock_output().
 * @retval LOGGER_ERROR Invalid context or mutex not created/acquired.
 * @note A current thread does not rule out ISR context, so IPSR is also checked.
 * Output availability is not checked because initialization uses this helper.
 */
static logger_status_t lock_output(void)
{
    if (!is_thread_context())
    {
        return LOGGER_ERROR;
    }
    if (!created)
    {
        return LOGGER_ERROR;
    }

    UINT status = tx_mutex_get(&output_mutex, LOGGER_WAIT_TICKS);
    return (status == TX_SUCCESS) ? LOGGER_OK : LOGGER_ERROR;
}

/**
 * @brief Release the mutex while preserving the completed operation result.
 * @param[in] result Result of the operation performed under the mutex.
 * @pre The current thread owns output_mutex.
 * @return result when release succeeds; otherwise LOGGER_ERROR.
 * @note A release failure does not undo output that already occurred.
 */
static logger_status_t unlock_output(logger_status_t result)
{
    return (tx_mutex_put(&output_mutex) == TX_SUCCESS) ? result : LOGGER_ERROR;
}

/**
 * @brief Wait until the previous transfer releases the shared buffer.
 * @pre The current thread owns output_mutex and output_ready is not NULL.
 */
static logger_status_t wait_output_ready(void)
{
    ULONG started = tx_time_get();

    while (!output_ready())
    {
        if ((ULONG)(tx_time_get() - started) >= LOGGER_WAIT_TICKS)
        {
            return LOGGER_ERROR;
        }
        if (tx_thread_sleep(1U) != TX_SUCCESS)
        {
            return LOGGER_ERROR;
        }
    }

    return LOGGER_OK;
}

/**
 * @brief Start asynchronous transmission of a prepared record.
 * @param[in] length Number of bytes to transmit from record, excluding NUL.
 * @pre The current thread owns output_mutex, output is attached, and length
 * does not exceed the record size.
 * @retval LOGGER_OK Output accepted the pointer to the complete array.
 * @retval LOGGER_ERROR Readiness timed out or the thread could not sleep.
 * @details Each attempt starts the complete transfer or starts nothing. The
 * next call does not modify record until output readiness is confirmed. Tick
 * subtraction handles counter wraparound.
 */
static logger_status_t send_record(size_t length)
{
    ULONG started = tx_time_get();

    while (!output_try_write((const uint8_t *)record, length))
    {
        if ((ULONG)(tx_time_get() - started) >= LOGGER_WAIT_TICKS)
        {
            return LOGGER_ERROR;
        }
        if (tx_thread_sleep(1U) != TX_SUCCESS)
        {
            return LOGGER_ERROR;
        }
    }

    return LOGGER_OK;
}

/**
 * @brief Build a prefix, printf-formatted body, and suffix, then send the record.
 * @param[in] prefix Non-NULL string before the body: time/level or ANSI color.
 * @param[in] suffix Non-NULL string after the body: CRLF, color reset, or empty.
 * @param[in] format Non-NULL printf format string.
 * @param[in] args Prepared argument list; the caller must later call va_end().
 * @pre The current thread owns output_mutex and output is attached. The prefix
 * and suffix leave at least four record bytes for an ellipsis and terminating NUL.
 * @retval LOGGER_OK Asynchronous transmission of the whole record started.
 * @retval LOGGER_ERROR The record was truncated or could not be built or sent.
 * @note A truncated record with an ellipsis is still transmitted.
 * @details Suffix space is reserved in advance. Truncation preserves the suffix
 * and a valid UTF-8 boundary; embedded ANSI commands are not parsed.
 */
static logger_status_t format_record(const char *prefix, const char *suffix,
                                     const char *format, va_list args)
{
    size_t prefix_length = strlen(prefix);
    size_t suffix_length = strlen(suffix);
    size_t body_capacity = sizeof(record) - prefix_length - suffix_length;
    bool truncated;

    memcpy(record, prefix, prefix_length);
    int length = vsnprintf(record + prefix_length, body_capacity, format, args);
    if (length < 0)
    {
        return LOGGER_ERROR;
    }

    truncated = (size_t)length >= body_capacity;
    size_t body_length = truncated ? body_capacity - 1U : (size_t)length;
    if (truncated)
    {
        /* Make room for the ellipsis, then step back over UTF-8 continuation
           bytes (10xxxxxx) so no code point is split. */
        body_length -= LOGGER_ELLIPSIS_LENGTH;
        while ((body_length > 0U) &&
               (((unsigned char)record[prefix_length + body_length] & 0xC0U) == 0x80U))
        {
            body_length--;
        }
        memcpy(record + prefix_length + body_length, LOGGER_ELLIPSIS, LOGGER_ELLIPSIS_LENGTH);
        body_length += LOGGER_ELLIPSIS_LENGTH;
    }

    memcpy(record + prefix_length + body_length, suffix, suffix_length);
    logger_status_t status = send_record(prefix_length + body_length + suffix_length);
    return truncated ? LOGGER_ERROR : status;
}

/**
 * @brief Common implementation for plain and colored printf output.
 * @param[in] color Validated logger_color_t value.
 * @param[in] format Non-NULL printf format string.
 * @param[in] args Caller-provided argument list; the caller later calls va_end().
 * @return Formatting/transmission result or an argument, readiness, context, or
 * mutex-operation error; see logger_print_color().
 * @details Acquires and releases output_mutex. Color and reset sequences are
 * added only when color differs from LOGGER_COLOR_DEFAULT.
 */
static logger_status_t print_text(logger_color_t color, const char *format, va_list args)
{
    if (((unsigned int)color >= sizeof(text_colors) / sizeof(text_colors[0])) ||
        (format == NULL))
    {
        return LOGGER_ERROR;
    }
    logger_status_t status = lock_output();
    if (status != LOGGER_OK)
    {
        return status;
    }
    if ((output_try_write == NULL) || (output_ready == NULL) ||
        (wait_output_ready() != LOGGER_OK))
    {
        return unlock_output(LOGGER_ERROR);
    }

    bool colored = (color != LOGGER_COLOR_DEFAULT);
    status = format_record(colored ? text_colors[color] : "",
                           colored ? ANSI_RESET : "", format, args);
    return unlock_output(status);
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

logger_status_t logger_init(logger_try_write_t try_write, logger_output_ready_t is_ready)
{
    if (!is_thread_context() || (try_write == NULL) || (is_ready == NULL))
    {
        return LOGGER_ERROR;
    }
    if (!created)
    {
        if (tx_mutex_create(&output_mutex, "logger_output", TX_INHERIT) != TX_SUCCESS)
        {
            return LOGGER_ERROR;
        }
        created = true;
    }

    logger_status_t status = lock_output();
    if (status != LOGGER_OK)
    {
        return status;
    }
    if ((output_ready != NULL) && (wait_output_ready() != LOGGER_OK))
    {
        return unlock_output(LOGGER_ERROR);
    }
    output_try_write = try_write;
    output_ready = is_ready;
    return unlock_output(LOGGER_OK);
}

logger_status_t logger_log(logger_level_t level, const char *format, ...)
{
    if (((unsigned int)level > LOGGER_LEVEL_INFO) || (format == NULL))
    {
        return LOGGER_ERROR;
    }
    logger_status_t status = lock_output();
    if (status != LOGGER_OK)
    {
        return status;
    }
    if ((output_try_write == NULL) || (output_ready == NULL) ||
        (wait_output_ready() != LOGGER_OK))
    {
        return unlock_output(LOGGER_ERROR);
    }
    ULONG ticks = tx_time_get();
    unsigned long seconds = ticks / TX_TIMER_TICKS_PER_SECOND;
    unsigned long milliseconds = (unsigned long)
        (((uint64_t)(ticks % TX_TIMER_TICKS_PER_SECOND) * 1000U) /
         TX_TIMER_TICKS_PER_SECOND);
    char prefix[64];
    int length = snprintf(prefix, sizeof(prefix), "%lus:%03lums [%s%-5s%s] ",
                          seconds, milliseconds,
                          level_colors[level],
                          level_names[level], ANSI_RESET);
    if ((length < 0) || ((size_t)length >= sizeof(prefix)))
    {
        return unlock_output(LOGGER_ERROR);
    }

    va_list args;
    va_start(args, format);
    status = format_record(prefix, "\r\n", format, args);
    va_end(args);
    return unlock_output(status);
}

logger_status_t logger_print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_status_t status = print_text(LOGGER_COLOR_DEFAULT, format, args);
    va_end(args);
    return status;
}

logger_status_t logger_print_color(logger_color_t color, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_status_t status = print_text(color, format, args);
    va_end(args);
    return status;
}

logger_status_t logger_erase(size_t columns)
{
    if (columns == 0U)
    {
        return LOGGER_OK;
    }
    /* CUB moves left; ECH erases without shifting the remaining text. */
    return logger_print("\033[%zuD\033[%zuX", columns, columns);
}

logger_status_t logger_clear_line(void)
{
    return logger_print("\r\033[2K");
}
