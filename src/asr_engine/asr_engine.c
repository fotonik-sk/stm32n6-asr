/**
 * @file     asr_engine.c
 * @brief    Creates, starts and wires the stages; implements asr_engine.h
 *           except asr_engine_audio_chunk_ready(), which is in mel_thread.c.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "asr_engine.h"

#include "asr_engine_conf.h"
#include "mel/mel_thread.h"
#include "net/net_npu.h"
#include "net/net_thread.h"
#include "norm/norm_thread.h"
#include "tx_api.h"

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/* Normalization state that lives for the whole stream; the host places it
   through ASR_CONF_NORM_MEMORY_ATTRIBUTES (PSRAM on this board). */

/** @brief Raw log-mel frames of the receptive field, normalized anew per window. */
static float norm_field[NORM_FIELD_VALUES] ASR_CONF_NORM_MEMORY_ATTRIBUTES;

/** @brief Per-block sums of the statistics memory. */
static norm_block_stats_t norm_stats[NORM_STATS_BLOCKS] ASR_CONF_NORM_MEMORY_ATTRIBUTES;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

uint32_t asr_engine_create(void)
{
    uint32_t status = mel_thread_create();

    if (status == TX_SUCCESS)
    {
        status = norm_thread_create();
    }
    if (status == TX_SUCCESS)
    {
        status = net_thread_create();
    }
    return status;
}

uint32_t asr_engine_start(const asr_audio_port_t *audio, const asr_text_port_t *text)
{
    norm_thread_output_t norm_output;
    uint32_t status;

    /* Consumers first, so each reader waits before its producer runs; the
       network also owns the tensor the windows are assembled in. */
    status = net_thread_start(text);
    if (status != TX_SUCCESS)
    {
        return status;
    }

    /* Normalization writes each window straight into the network input tensor
       and hands it over through the network thread's two hooks. */
    norm_output.buffers.input = net_npu_input();
    norm_output.buffers.field = norm_field;
    norm_output.buffers.stats = norm_stats;
    norm_output.acquire_input = net_thread_acquire_input;
    norm_output.window_ready = net_thread_window_ready;

    status = norm_thread_start(&norm_output);
    if (status != TX_SUCCESS)
    {
        return status;
    }

    return mel_thread_start(audio, norm_thread_frames_ready);
}
