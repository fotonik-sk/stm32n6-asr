/**
 * @file     audio_port.h
 * @brief    Microphone, plugged into asr_engine as its audio port.
 * @details  Starts the microphone (bsp_mic + drv_pdm) with the capture
 *           geometry asr_engine needs and exposes the PDM driver as an
 *           asr_audio_port_t.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef APPLICATION_AUDIO_AUDIO_PORT_H
#define APPLICATION_AUDIO_AUDIO_PORT_H

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
 * @brief Start the microphone; every captured chunk wakes asr_engine.
 * @retval 0 Capture is running; asr_engine_start() may be called.
 * @retval non-zero The drv_pdm_status_t of the failing step.
 * @pre asr_engine_create() has run, so the chunk interrupt has a thread to wake.
 */
uint32_t audio_port_start(void);

/**
 * @brief Return the audio port backed by the PDM driver; pass it to asr_engine_start().
 * @return A pointer valid for the life of the program.
 */
const asr_audio_port_t *audio_port_get(void);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_AUDIO_AUDIO_PORT_H */
