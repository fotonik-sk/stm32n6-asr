/**
 * @file     mel.c
 * @brief    Streaming log-mel converter; implements mel.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mel.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

#include "mel_fft.h"
#include "mel_tables.h"

/*******************************************************************************/
/*                              STATIC ASSERTIONS                              */
/*******************************************************************************/

_Static_assert(ASR_MODEL_MEL_HOP_SAMPLES <= ASR_MODEL_MEL_FRAME_SAMPLES,
               "the hop cannot exceed the frame length");
_Static_assert(ASR_MODEL_MEL_FRAME_SAMPLES <= ASR_MODEL_MEL_FFT_SIZE,
               "the frame must fit the zero-padded FFT buffer");
_Static_assert((ASR_MODEL_MEL_FFT_SIZE & (ASR_MODEL_MEL_FFT_SIZE - 1U)) == 0U,
               "the radix-2 FFT needs a power-of-two length");

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief Full-scale magnitude of a PCM16 sample. */
#define MEL_PCM16_FULL_SCALE 32768.0f

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Pre-emphasized samples of the frame being assembled, overlap included. */
static float hold[ASR_MODEL_MEL_FRAME_SAMPLES];

/** @brief Valid entries at the front of hold. */
static uint32_t pending;

/** @brief Last raw sample of the stream, the @c x[n-1] of the next block. */
static float previous_sample;

/** @brief Cleared until the first sample of a stream is seen. */
static int stream_started;

/** @brief Set once a frame has been emitted since the last reset. */
static int frame_emitted;

/** @brief Set once the tables are built. */
static int ready;

/** @brief Windowed and zero-padded frame handed to the FFT. */
static float fft_input[ASR_MODEL_MEL_FFT_SIZE];

/** @brief Power spectrum of the current frame. */
static float power[MEL_SPECTRUM_BINS];

/** @brief Log-mel values of the current frame. */
static float frame_bands[ASR_MODEL_MEL_BANDS];

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Run window, FFT, mel bank and logarithm over one assembled frame.
 * @param[in] frame @c ASR_MODEL_MEL_FRAME_SAMPLES pre-emphasized samples.
 * @param[out] bands @c ASR_MODEL_MEL_BANDS log-mel values.
 */
static void convert_frame(const float *frame, float *bands)
{
    const float *window = mel_tables_window();
    const mel_filter_span_t *filters = mel_tables_filters();
    const float *weights = mel_tables_weights();
    uint32_t index;
    uint32_t band;

    for (index = 0U; index < ASR_MODEL_MEL_FRAME_SAMPLES; ++index)
    {
        fft_input[index] = frame[index] * window[index];
    }
    for (index = ASR_MODEL_MEL_FRAME_SAMPLES; index < ASR_MODEL_MEL_FFT_SIZE; ++index)
    {
        fft_input[index] = 0.0f;
    }

    mel_fft_power(fft_input, power);

    for (band = 0U; band < ASR_MODEL_MEL_BANDS; ++band)
    {
        const float *band_weights = &weights[filters[band].offset];
        const float *band_power = &power[filters[band].start];
        float accumulator = 0.0f;

        for (index = 0U; index < filters[band].count; ++index)
        {
            accumulator += band_power[index] * band_weights[index];
        }
        bands[band] = logf(accumulator + ASR_MODEL_MEL_LOG_ZERO_GUARD);
    }
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

mel_status_t mel_init(void)
{
    if (mel_tables_init() != 0)
    {
        return MEL_ERROR;
    }
    mel_fft_init();
    ready = 1;
    mel_reset();
    return MEL_OK;
}

void mel_reset(void)
{
    pending = 0U;
    previous_sample = 0.0f;
    stream_started = 0;
    frame_emitted = 0;
}

mel_status_t mel_process(const int16_t *pcm,
                         uint32_t samples,
                         mel_frame_callback_t on_frame,
                         void *context)
{
    uint32_t index;

    if (!ready || (on_frame == NULL) || ((pcm == NULL) && (samples != 0U)))
    {
        return MEL_ERROR;
    }

    for (index = 0U; index < samples; ++index)
    {
        float sample = (float)pcm[index] / MEL_PCM16_FULL_SCALE;
        float emphasized;

        /* Pre-emphasis spans blocks; only a stream's first sample passes as is. */
        if (stream_started)
        {
            emphasized = sample - (ASR_MODEL_MEL_PREEMPHASIS * previous_sample);
        }
        else
        {
            emphasized = sample;
            stream_started = 1;
        }
        previous_sample = sample;

        hold[pending] = emphasized;
        ++pending;
        if (pending == ASR_MODEL_MEL_FRAME_SAMPLES)
        {
            convert_frame(hold, frame_bands);
            frame_emitted = 1;
            on_frame(frame_bands, context);

            /* Slide by one hop: the overlap starts the next frame. */
            memmove(hold,
                    &hold[ASR_MODEL_MEL_HOP_SAMPLES],
                    MEL_FRAME_OVERLAP_SAMPLES * sizeof(hold[0]));
            pending = MEL_FRAME_OVERLAP_SAMPLES;
        }
    }
    return MEL_OK;
}

mel_status_t mel_flush(mel_frame_callback_t on_frame, void *context)
{
    if (!ready || (on_frame == NULL))
    {
        return MEL_ERROR;
    }

    /* Pad only a stream shorter than one frame; drop a tail after real frames. */
    if (!frame_emitted && (pending > 0U))
    {
        memset(&hold[pending],
               0,
               (ASR_MODEL_MEL_FRAME_SAMPLES - pending) * sizeof(hold[0]));
        convert_frame(hold, frame_bands);
        on_frame(frame_bands, context);
    }
    mel_reset();
    return MEL_OK;
}

uint32_t mel_pending_samples(void)
{
    return pending;
}

mel_status_t mel_frame_to_logmel(const float *frame, float *bands)
{
    if (!ready || (frame == NULL) || (bands == NULL))
    {
        return MEL_ERROR;
    }
    convert_frame(frame, bands);
    return MEL_OK;
}
