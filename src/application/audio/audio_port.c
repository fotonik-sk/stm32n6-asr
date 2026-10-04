/**
 * @file     audio_port.c
 * @brief    Microphone as the asr_engine audio port; implements audio_port.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "audio_port.h"

#include <stddef.h>

#include "asr_engine.h"
#include "bsp_mic.h"

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/** @brief Wake the conversion thread once per completed capture chunk. */
static void chunk_ready_callback(void *context)
{
  (void)context;
  asr_engine_audio_chunk_ready();
}

/** @brief Forward to drv_pdm_available_chunks(). */
static uint32_t port_available_chunks(void *context)
{
  (void)context;
  return drv_pdm_available_chunks();
}

/** @brief Forward to drv_pdm_read(), with its status as the driver-agnostic asr_audio_status_t. */
static asr_audio_status_t port_read(void *context,
                                    int16_t *destination,
                                    uint32_t destination_samples,
                                    uint32_t chunks)
{
  (void)context;
  switch (drv_pdm_read(destination, destination_samples, chunks))
  {
  case DRV_PDM_OK:
    return ASR_AUDIO_OK;
  case DRV_PDM_UNDERRUN:
    return ASR_AUDIO_UNDERRUN;
  case DRV_PDM_OVERRUN:
    return ASR_AUDIO_OVERRUN;
  default:
    return ASR_AUDIO_ERROR;
  }
}

/** @brief Forward to drv_pdm_take_dropped_chunks(). */
static uint32_t port_take_dropped_chunks(void *context)
{
  (void)context;
  return drv_pdm_take_dropped_chunks();
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

uint32_t audio_port_start(void)
{
  drv_pdm_config_t config = {0};

  config.chunk_samples = ASR_MODEL_MEL_HOP_SAMPLES;
  config.chunk_count = ASR_CONF_CAPTURE_CHUNK_COUNT;
  config.chunk_callback = chunk_ready_callback;
  config.callback_context = NULL;
  return (uint32_t)bsp_mic_start(&config);
}

const asr_audio_port_t *audio_port_get(void)
{
  /** @brief The microphone's plug into the model layer; the PDM driver is a singleton. */
  static const asr_audio_port_t mic_port = {
    .context = NULL,
    .available_chunks = port_available_chunks,
    .read = port_read,
    .take_dropped_chunks = port_take_dropped_chunks,
  };

  return &mic_port;
}
