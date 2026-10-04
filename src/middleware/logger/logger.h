 /**
 * @file     logger.h
 * @brief    Thread-safe text logger for ThreadX and ANSI terminals.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef MIDDLEWARE_LOGGER_H
#define MIDDLEWARE_LOGGER_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/**
 * @def LOGGER_PRINTF(format_index, args_index)
 * @brief Enable printf-format checking with GCC or Clang.
 * @param format_index One-based index of the format-string parameter.
 * @param args_index One-based index of the first variadic argument.
 * @note Expands to nothing for other compilers.
 */
#if defined(__GNUC__) || defined(__clang__)
#define LOGGER_PRINTF(format_index, args_index) \
    __attribute__((format(printf, format_index, args_index)))
#else
#define LOGGER_PRINTF(format_index, args_index)
#endif

/**
 * @brief Write a critical error with the CRIT label.
 * @param ... printf format string and matching arguments.
 * @return Result of logger_log(); the application is not stopped automatically.
 */
#define LOG_CRITICAL(...) logger_log(LOGGER_LEVEL_CRITICAL, __VA_ARGS__)

/**
 * @brief Write an error with the ERROR label.
 * @param ... printf format string and matching arguments.
 * @return Result of logger_log().
 */
#define LOG_ERROR(...) logger_log(LOGGER_LEVEL_ERROR, __VA_ARGS__)

/**
 * @brief Write a warning with the WARN label.
 * @param ... printf format string and matching arguments.
 * @return Result of logger_log().
 */
#define LOG_WARNING(...) logger_log(LOGGER_LEVEL_WARNING, __VA_ARGS__)

/**
 * @brief Write an informational message with the INFO label.
 * @param ... printf format string and matching arguments.
 * @return Result of logger_log().
 */
#define LOG_INFO(...) logger_log(LOGGER_LEVEL_INFO, __VA_ARGS__)

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/**
 * @brief Record severity level.
 * @details A lower value indicates greater severity. All levels are always emitted.
 */
typedef enum
{
    LOGGER_LEVEL_CRITICAL, /**< Critical error: bold red CRIT label. */
    LOGGER_LEVEL_ERROR,    /**< Error: red ERROR label. */
    LOGGER_LEVEL_WARNING,  /**< Warning: yellow WARN label. */
    LOGGER_LEVEL_INFO      /**< Information: green INFO label. */
} logger_level_t;

/** @brief Plain-text color used by logger_print_color(). */
typedef enum
{
    LOGGER_COLOR_DEFAULT, /**< Do not add color control sequences. */
    LOGGER_COLOR_RED,     /**< Red. */
    LOGGER_COLOR_GREEN,   /**< Green. */
    LOGGER_COLOR_YELLOW,  /**< Yellow. */
    LOGGER_COLOR_BLUE,    /**< Blue. */
    LOGGER_COLOR_MAGENTA, /**< Magenta. */
    LOGGER_COLOR_CYAN,    /**< Cyan. */
    LOGGER_COLOR_WHITE    /**< White. */
} logger_color_t;

/** @brief Logger operation result. */
typedef enum
{
    LOGGER_OK = 0, /**< Operation succeeded; transmission of the full text started. */
    LOGGER_ERROR   /**< Operation was incomplete; truncated text may be transmitting. */
} logger_status_t;

/**
 * @brief Start asynchronous transmission of a complete record.
 * @param[in] data Record bytes; the pointer remains valid until output is ready.
 * @param[in] length Number of bytes in data.
 * @return true if transmission of the whole array started; false if none started.
 * The function must neither wait nor copy the array.
 */
typedef bool (*logger_try_write_t)(const uint8_t *data, size_t length);

/**
 * @brief Check whether asynchronous transmission is complete.
 * @return true when output no longer uses the pointer passed to it.
 * @note The function must not wait.
 */
typedef bool (*logger_output_ready_t)(void);

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Create the priority-inheriting mutex and attach output callbacks.
 * @param[in] try_write Preinitialized transfer-start callback.
 * @param[in] is_ready Transfer-completion callback.
 * @pre Only the startup thread makes the first call, before log consumers start.
 * @retval LOGGER_OK Logger is ready, including after a repeated call.
 * @retval LOGGER_ERROR Invalid context, NULL callback, or mutex operation failure.
 * @note Initial setup must not run concurrently. Check the result before starting
 * dependent threads; failure is a system startup failure.
 * @note A repeated call waits for the old output and replaces callbacks under mutex.
 */
logger_status_t logger_init(logger_try_write_t try_write, logger_output_ready_t is_ready);

/**
 * @brief Write a message with uptime, severity level, and trailing CRLF.
 * @param[in] level Level from LOGGER_LEVEL_CRITICAL through LOGGER_LEVEL_INFO.
 * @param[in] format Non-NULL printf format string without a trailing newline.
 * @param ... Arguments matching the format specifiers.
 * @retval LOGGER_OK Transmission of the complete record started.
 * @retval LOGGER_ERROR Text was truncated or an argument, context, readiness,
 * formatting, output-wait, or mutex operation failed.
 * @note Transmission may already have started after truncation or unlock failure.
 * @details Example: `1s:402ms [INFO ] Ready`. The level label occupies five
 * columns and only it is colored. File name and line number are omitted. Time is
 * the ThreadX tick count since kernel startup, not calendar time; at 1000 Hz a
 * 32-bit counter wraps after approximately 49.7 days.
 * @note CRITICAL expresses severity and does not stop the application.
 */
logger_status_t logger_log(logger_level_t level, const char *format, ...)
    LOGGER_PRINTF(2, 3);

/**
 * @brief Write formatted text without metadata.
 * @param[in] format Non-NULL printf format string; use CRLF for a line break.
 * @param ... Arguments matching the format specifiers.
 * @retval LOGGER_OK Transmission of all text started.
 * @retval LOGGER_ERROR Text was truncated or an argument, context, readiness,
 * formatting, output-wait, or mutex operation failed.
 * @note Transmission may already have started after truncation or unlock failure.
 * @note Does not add time, level, color, or a newline. Pass external text as
 * logger_print("%s", text).
 */
logger_status_t logger_print(const char *format, ...) LOGGER_PRINTF(1, 2);

/**
 * @brief Write text in the selected color without time or severity metadata.
 * @param[in] color Text color; LOGGER_COLOR_DEFAULT adds no ANSI color codes.
 * @param[in] format Non-NULL printf format string; no newline is added.
 * @param ... Arguments matching the format specifiers.
 * @retval LOGGER_OK Transmission of all colored text started.
 * @retval LOGGER_ERROR Text was truncated or an argument, context, readiness,
 * formatting, output-wait, or mutex operation failed.
 * @note Transmission may already have started after truncation or unlock failure.
 * @details An explicit color appends an attribute reset even after truncation.
 * LOGGER_COLOR_DEFAULT behaves like logger_print(). Reset restores default
 * attributes rather than the previous color.
 */
logger_status_t logger_print_color(logger_color_t color, const char *format, ...)
    LOGGER_PRINTF(2, 3);

/**
 * @brief Erase a number of terminal columns to the left of the cursor.
 * @param[in] columns Number of visible columns, not UTF-8 bytes; zero does nothing.
 * @pre For nonzero columns, the cursor is at least that far from the beginning
 * of the current line. The logger does not track cursor position.
 * @retval LOGGER_OK Command transmission started or columns is zero.
 * @retval LOGGER_ERROR The command could not be completed.
 * @details ANSI CUB moves left and ECH replaces the requested columns with
 * spaces. The cursor remains at the beginning of the erased region. Other lines
 * and terminal history are unaffected.
 * @note Another thread may emit text between separate print/erase calls.
 */
logger_status_t logger_erase(size_t columns);

/**
 * @brief Clear the current terminal line and move the cursor to its beginning.
 * @retval LOGGER_OK Transmission of the CR plus ANSI EL(2) sequence started.
 * @retval LOGGER_ERROR The command could not be completed.
 */
logger_status_t logger_clear_line(void);

#ifdef __cplusplus
}
#endif

#endif /* MIDDLEWARE_LOGGER_H */
