/**
 * @file     mel_thread.h
 * @brief    Thread of the mel stage: audio port to log-mel features.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_MEL_MEL_THREAD_H
#define ASR_ENGINE_MEL_MEL_THREAD_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "asr_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Called on the mel thread after each frame stored in mel_ring; must not block. */
typedef void (*mel_thread_frames_hook_t)(void);

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Create the thread and its chunk semaphore, suspended.
 * @return TX_SUCCESS, or the ThreadX error of the failing step.
 * @pre Call before the scheduler starts.
 */
uint32_t mel_thread_create(void);

/**
 * @brief Initialize the converter and the feature ring, then resume the thread.
 * @param[in] port Capture backend; complete, must outlive the thread.
 * @param[in] on_frames Wakes the mel_ring consumer, or NULL if there is none.
 * @return TX_SUCCESS, or non-zero on failure; the reason is logged.
 * @pre The logger and the capture are running. Call once, from another thread.
 */
uint32_t mel_thread_start(const asr_audio_port_t *port, mel_thread_frames_hook_t on_frames);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_MEL_MEL_THREAD_H */
