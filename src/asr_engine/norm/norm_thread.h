/**
 * @file     norm_thread.h
 * @brief    Thread of the normalization stage: log-mel frames to network windows.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_NORM_NORM_THREAD_H
#define ASR_ENGINE_NORM_NORM_THREAD_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "norm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Consumer of the windows, wired up by asr_engine.c. */
typedef struct
{
    /** @brief Network input tensor, raw field and statistics ring. */
    norm_buffers_t buffers;

    /** @brief Block until nothing reads @c buffers.input. */
    void (*acquire_input)(void);

    /** @brief Signal that @c buffers.input holds a new window. */
    void (*window_ready)(void);
} norm_thread_output_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Create the thread and its frames semaphore, suspended.
 * @return TX_SUCCESS, or the ThreadX error of the failing step.
 * @pre Call before the scheduler starts.
 */
uint32_t norm_thread_create(void);

/**
 * @brief Bind the buffers, start a new stream and resume the thread.
 * @param[in] output Buffers and hooks, all mandatory; copied.
 * @return TX_SUCCESS, or non-zero on failure; the reason is logged.
 * @pre The consumer is ready. Call once, from another thread, before the mel stage.
 */
uint32_t norm_thread_start(const norm_thread_output_t *output);

/**
 * @brief Signal one frame stored in mel_ring; a mel_thread_frames_hook_t.
 * @note Harmless before norm_thread_start().
 */
void norm_thread_frames_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_NORM_NORM_THREAD_H */
