/**
 * @file     norm_thread.c
 * @brief    Thread of the normalization stage; implements norm_thread.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "norm_thread.h"

#include <stddef.h>

#include "asr_engine_conf.h"
#include "logger.h"
#include "mel_ring.h"
#include "norm.h"
#include "tx_api.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief norm_thread_start() status for a failed non-RTOS step; no TX_* code uses it. */
#define NORM_THREAD_INIT_ERROR 0xFFFFFFFFU

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Control block of the normalization thread. */
static TX_THREAD norm_thread;

/** @brief Private stack of the normalization thread. */
static ULONG norm_thread_stack[ASR_CONF_NORM_THREAD_STACK_BYTES / sizeof(ULONG)];

/** @brief Raised once per feature vector the mel stage stores. */
static TX_SEMAPHORE frames_semaphore;

/** @brief One frame taken out of the mel ring. */
static float frame[ASR_MODEL_MEL_BANDS];

/** @brief Consumer of the windows, bound by norm_thread_start(). */
static norm_thread_output_t window_consumer;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Feed every frame waiting in the mel ring to the normalizer.
 */
static void drain_feature_ring(void)
{
    while (mel_ring_available() != 0U)
    {
        mel_ring_status_t status = mel_ring_read(frame, ASR_MODEL_MEL_BANDS, 1U);

        if (status == MEL_RING_OVERRUN)
        {
            /* The ring has already skipped ahead to its newest frames. */
            LOG_WARNING("Normalization lost a log-mel frame: the feature ring overran");
            continue;
        }
        if (status != MEL_RING_OK)
        {
            LOG_ERROR("Normalization could not read the feature ring (status %d)", (int)status);
            return;
        }

        /* A frame that closes a block overwrites the network input: wait for inference. */
        if (norm_next_frame_writes_window())
        {
            window_consumer.acquire_input();
        }
        if (norm_push_frame(frame) == NORM_WINDOW_READY)
        {
            window_consumer.window_ready();
        }
    }
}

/**
 * @brief Normalize log-mel frames for as long as the board runs.
 * @param[in] argument Unused ThreadX entry argument.
 */
static void norm_thread_entry(ULONG argument)
{
    (void)argument;

    for (;;)
    {
        /* One token per frame; a wake-up drains all, so later ones may find none. */
        if (tx_semaphore_get(&frames_semaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
        {
            continue;
        }
        drain_feature_ring();
    }
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

uint32_t norm_thread_create(void)
{
    UINT status;

    status = tx_semaphore_create(&frames_semaphore, "asr_norm_semaphore", 0U);
    if (status != TX_SUCCESS)
    {
        return (uint32_t)status;
    }
    return (uint32_t)tx_thread_create(&norm_thread,
                                      "asr_norm_thread",
                                      norm_thread_entry,
                                      0U,
                                      norm_thread_stack,
                                      sizeof(norm_thread_stack),
                                      ASR_CONF_NORM_THREAD_PRIORITY,
                                      ASR_CONF_NORM_THREAD_PRIORITY,
                                      TX_NO_TIME_SLICE,
                                      TX_DONT_START);
}

uint32_t norm_thread_start(const norm_thread_output_t *output)
{
    if ((output == NULL) || (output->acquire_input == NULL) || (output->window_ready == NULL))
    {
        LOG_ERROR("Normalization output is incomplete");
        return NORM_THREAD_INIT_ERROR;
    }
    if (norm_init(&output->buffers) != NORM_OK)
    {
        LOG_ERROR("Normalization initialization failed");
        return NORM_THREAD_INIT_ERROR;
    }
    window_consumer = *output;

    return (uint32_t)tx_thread_resume(&norm_thread);
}

void norm_thread_frames_ready(void)
{
    (void)tx_semaphore_put(&frames_semaphore);
}
