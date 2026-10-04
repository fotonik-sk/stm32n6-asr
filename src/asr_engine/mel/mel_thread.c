/**
 * @file     mel_thread.c
 * @brief    Thread of the mel stage; implements mel_thread.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mel_thread.h"

#include <stddef.h>

#include "asr_engine.h"
#include "logger.h"
#include "mel.h"
#include "mel_cfg.h"
#include "mel_ring.h"
#include "tx_api.h"

/*******************************************************************************/
/*                              STATIC ASSERTIONS                              */
/*******************************************************************************/

_Static_assert(ASR_CONF_CAPTURE_CHUNK_COUNT >= 2U,
               "the capture ring needs a chunk for the DMA and one for the reader");

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief mel_thread_start() status for a failed non-RTOS step; no TX_* code uses it. */
#define MEL_THREAD_INIT_ERROR 0xFFFFFFFFU

/** @brief Staging capacity in samples: the driver always owns one chunk of the ring. */
#define MEL_THREAD_PCM_SAMPLES ((ASR_CONF_CAPTURE_CHUNK_COUNT - 1U) * ASR_MODEL_MEL_HOP_SAMPLES)

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Control block of the conversion thread. */
static TX_THREAD mel_thread;

/** @brief Private stack of the conversion thread. */
static ULONG mel_thread_stack[ASR_CONF_MEL_THREAD_STACK_BYTES / sizeof(ULONG)];

/** @brief Raised once per captured chunk, from the capture interrupt. */
static TX_SEMAPHORE chunk_semaphore;

/** @brief Staging area for the PCM16 samples handed to the converter. */
static int16_t pcm[MEL_THREAD_PCM_SAMPLES];

/** @brief Capture backend bound by mel_thread_start(); valid once the thread runs. */
static const asr_audio_port_t *audio_port;

/** @brief Consumer wake-up bound by mel_thread_start(); NULL when nothing reads the ring. */
static mel_thread_frames_hook_t frames_hook;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Store one finished feature vector in the ring and wake its consumer.
 * @param[in] bands @c ASR_MODEL_MEL_BANDS log-mel values.
 * @param[in,out] context Unused.
 */
static void frame_ready_callback(const float *bands, void *context)
{
    (void)context;
    mel_ring_write(bands);
    if (frames_hook != NULL)
    {
        frames_hook();
    }
}

/**
 * @brief Drain the capture source and convert everything it holds.
 */
static void convert_available_chunks(void)
{
    /* The capture driver always owns the chunk it is filling. */
    const uint32_t max_chunks = ASR_CONF_CAPTURE_CHUNK_COUNT - 1U;
    uint32_t chunks = audio_port->available_chunks(audio_port->context);
    asr_audio_status_t status;

    if (chunks == 0U)
    {
        return;
    }
    if (chunks > max_chunks)
    {
        chunks = max_chunks;
    }

    status = audio_port->read(audio_port->context, pcm, MEL_THREAD_PCM_SAMPLES, chunks);
    if (status == ASR_AUDIO_OVERRUN)
    {
        /* Samples were lost during the copy: reset, so no filter state spans the gap. */
        LOG_WARNING("Mel front-end lost audio: capture read raced the driver (%lu chunks)",
                    (unsigned long)chunks);
        mel_reset();
        return;
    }
    if (status != ASR_AUDIO_OK)
    {
        /* No audio was consumed, so the converter state stays valid. */
        LOG_ERROR("Mel front-end could not read %lu capture chunks", (unsigned long)chunks);
        return;
    }
    (void)mel_process(pcm, chunks * ASR_MODEL_MEL_HOP_SAMPLES, frame_ready_callback, NULL);
}

/**
 * @brief Convert captured audio into log-mel frames for as long as the board runs.
 * @param[in] argument Unused ThreadX entry argument.
 */
static void mel_thread_entry(ULONG argument)
{
    (void)argument;

    for (;;)
    {
        uint32_t dropped;

        /* One token per chunk; a wake-up drains all, so later ones may find none. */
        if (tx_semaphore_get(&chunk_semaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
        {
            continue;
        }

        dropped = audio_port->take_dropped_chunks(audio_port->context);
        if (dropped != 0U)
        {
            LOG_WARNING("Mel front-end lost audio: conversion fell behind capture (%lu chunks)",
                        (unsigned long)dropped);
            mel_reset();
        }

        convert_available_chunks();

        dropped = mel_ring_take_dropped();
        if (dropped != 0U)
        {
            LOG_WARNING("Mel feature ring overflowed: %lu frames dropped",
                        (unsigned long)dropped);
        }
    }
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

uint32_t mel_thread_create(void)
{
    UINT status;

    status = tx_semaphore_create(&chunk_semaphore, "asr_chunk_semaphore", 0U);
    if (status != TX_SUCCESS)
    {
        return (uint32_t)status;
    }
    return (uint32_t)tx_thread_create(&mel_thread,
                                      "asr_mel_thread",
                                      mel_thread_entry,
                                      0U,
                                      mel_thread_stack,
                                      sizeof(mel_thread_stack),
                                      ASR_CONF_MEL_THREAD_PRIORITY,
                                      ASR_CONF_MEL_THREAD_PRIORITY,
                                      TX_NO_TIME_SLICE,
                                      TX_DONT_START);
}

uint32_t mel_thread_start(const asr_audio_port_t *port, mel_thread_frames_hook_t on_frames)
{
    if ((port == NULL) ||
        (port->available_chunks == NULL) ||
        (port->read == NULL) ||
        (port->take_dropped_chunks == NULL))
    {
        LOG_ERROR("Audio port is incomplete");
        return MEL_THREAD_INIT_ERROR;
    }
    audio_port = port;
    frames_hook = on_frames;

    if (mel_init() != MEL_OK)
    {
        LOG_ERROR("Mel front-end initialization failed");
        return MEL_THREAD_INIT_ERROR;
    }
    mel_ring_reset();

    return (uint32_t)tx_thread_resume(&mel_thread);
}

/* Declared in asr_engine.h: the chunk semaphore lives here. */
void asr_engine_audio_chunk_ready(void)
{
    (void)tx_semaphore_put(&chunk_semaphore);
}
