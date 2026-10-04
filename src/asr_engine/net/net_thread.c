/**
 * @file     net_thread.c
 * @brief    Thread of the network stage; implements net_thread.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "net_thread.h"

#include <stddef.h>

#include "asr_engine_conf.h"
#include "logger.h"
#include "net_ctc.h"
#include "net_npu.h"
#include "tx_api.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief net_thread_start() status for a failed non-RTOS step; no TX_* code uses it. */
#define NET_THREAD_INIT_ERROR 0xFFFFFFFFU

/** @brief How long a writer of the input tensor waits between two stall reports. */
#define NET_THREAD_INPUT_WAIT_TICKS (2U * TX_TIMER_TICKS_PER_SECOND)

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Control block of the network thread. */
static TX_THREAD net_thread;

/** @brief Private stack of the network thread. */
static ULONG net_thread_stack[ASR_CONF_NET_THREAD_STACK_BYTES / sizeof(ULONG)];

/** @brief One token per window normalization hands over. */
static TX_SEMAPHORE window_semaphore;

/** @brief One token while no inference reads the input tensor. */
static TX_SEMAPHORE input_semaphore;

/** @brief Destination of the text; NULL when it is only logged. */
static const asr_text_port_t *text_port;

/** @brief CTC decoder state carried from one window to the next. */
static net_ctc_state_t ctc;

/** @brief Final text of the last window. */
static char committed[NET_TEXT_CAPACITY];

/** @brief Tentative text of the last window. */
static char tentative[NET_TEXT_CAPACITY];

/** @brief Windows decoded since net_thread_start(). */
static uint32_t windows;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Run the network over the window in the input tensor and publish its text.
 */
static void run_window(void)
{
    const int first = (windows == 0U);
    ULONG started;
    ULONG elapsed;
    net_npu_status_t status;

    started = tx_time_get();
    status = net_npu_run(first);
    elapsed = tx_time_get() - started;

    if (status == NET_NPU_OK)
    {
        net_ctc_decode_window(&ctc,
                              net_npu_output(),
                              first,
                              committed,
                              sizeof(committed),
                              tentative,
                              sizeof(tentative));
    }

    /* The output has been read, so the next window may go into the input. */
    (void)tx_semaphore_put(&input_semaphore);

    if (status != NET_NPU_OK)
    {
        LOG_ERROR("Network inference failed");
        return;
    }

    ++windows;
    if (first)
    {
        LOG_INFO("NPU inference time of one window: %lu ms",
                 (unsigned long)((elapsed * 1000U) / TX_TIMER_TICKS_PER_SECOND));
    }

    if (text_port != NULL)
    {
        text_port->publish(text_port->context, committed, tentative);
    }
    /* The log gets the bare committed text, so windows join into one transcript. */
    if (committed[0] != '\0')
    {
        (void)logger_print("%s", committed);
    }
}

/**
 * @brief Recognize every window normalization hands over.
 * @param[in] argument Unused ThreadX entry argument.
 */
static void net_thread_entry(ULONG argument)
{
    (void)argument;

    for (;;)
    {
        if (tx_semaphore_get(&window_semaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
        {
            continue;
        }
        run_window();
    }
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

uint32_t net_thread_create(void)
{
    UINT status;

    status = tx_semaphore_create(&window_semaphore, "asr_net_window", 0U);
    if (status != TX_SUCCESS)
    {
        return (uint32_t)status;
    }
    /* The input is free until the first window is written. */
    status = tx_semaphore_create(&input_semaphore, "asr_net_input", 1U);
    if (status != TX_SUCCESS)
    {
        return (uint32_t)status;
    }
    return (uint32_t)tx_thread_create(&net_thread,
                                      "asr_net_thread",
                                      net_thread_entry,
                                      0U,
                                      net_thread_stack,
                                      sizeof(net_thread_stack),
                                      ASR_CONF_NET_THREAD_PRIORITY,
                                      ASR_CONF_NET_THREAD_PRIORITY,
                                      TX_NO_TIME_SLICE,
                                      TX_DONT_START);
}

uint32_t net_thread_start(const asr_text_port_t *text)
{
    if ((text != NULL) && (text->publish == NULL))
    {
        LOG_ERROR("Text port is incomplete");
        return NET_THREAD_INIT_ERROR;
    }
    text_port = text;

    if (net_npu_init() != NET_NPU_OK)
    {
        LOG_ERROR("NPU runtime or network initialization failed");
        return NET_THREAD_INIT_ERROR;
    }
    net_ctc_reset(&ctc);
    windows = 0U;

    return (uint32_t)tx_thread_resume(&net_thread);
}

void net_thread_acquire_input(void)
{
    unsigned long waited_s = 0U;

    while (tx_semaphore_get(&input_semaphore, NET_THREAD_INPUT_WAIT_TICKS) != TX_SUCCESS)
    {
        waited_s += NET_THREAD_INPUT_WAIT_TICKS / TX_TIMER_TICKS_PER_SECOND;
        LOG_WARNING("Waited %lu s for the network input: inference %lu",
                    waited_s,
                    (unsigned long)(windows + 1U));
    }
}

void net_thread_window_ready(void)
{
    (void)tx_semaphore_put(&window_semaphore);
}
