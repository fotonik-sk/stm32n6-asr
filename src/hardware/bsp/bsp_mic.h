/**
 * @file     bsp_mic.h
 * @brief    Board microphone: the on-board MP34DT06J PDM microphone.
 * @details  Start-up only; after it succeeds the audio path is the drv_pdm.h API
 *           (drv_pdm_read(), drv_pdm_available_chunks(), ...).
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_BSP_BSP_MIC_H
#define HARDWARE_BSP_BSP_MIC_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "drv_pdm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Route the microphone pins and start continuous 16 kHz PCM16 capture.
 * @param[in] capture Chunk geometry and optional chunk notification. The board
 *            fields (@c init_pins, @c irq_priority) are ignored and replaced
 *            with this board's values.
 * @return The drv_pdm_init() status; see drv_pdm.h for the geometry limits and
 *         the once-per-reset rule.
 */
drv_pdm_status_t bsp_mic_start(const drv_pdm_config_t *capture);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_BSP_BSP_MIC_H */
