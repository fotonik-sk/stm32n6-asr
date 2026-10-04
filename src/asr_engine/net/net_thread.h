/**
 * @file     net_thread.h
 * @brief    Thread of the network stage: normalized windows to recognized text.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_NET_NET_THREAD_H
#define ASR_ENGINE_NET_NET_THREAD_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "asr_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Create the thread and its semaphores, suspended.
 * @return TX_SUCCESS, or the ThreadX error of the failing step.
 * @pre Call before the scheduler starts.
 */
uint32_t net_thread_create(void);

/**
 * @brief Start the NPU runtime, bind the network and resume the thread.
 * @param[in] text Text destination, or NULL to only log it; must outlive the layer.
 * @return TX_SUCCESS, or non-zero on failure; the reason is logged.
 * @pre The NPU is up (asr_engine.h). Call once, from another thread, before
 *      the normalization stage.
 */
uint32_t net_thread_start(const asr_text_port_t *text);

/**
 * @brief Wait until no inference reads the input tensor.
 * @note Called by normalization before it writes a window; warns every 2 s of waiting.
 */
void net_thread_acquire_input(void);

/**
 * @brief Hand the thread the window just written into the input tensor.
 * @pre net_thread_acquire_input() was called before the window was written.
 */
void net_thread_window_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_NET_NET_THREAD_H */
