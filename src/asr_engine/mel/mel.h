/**
 * @file     mel.h
 * @brief    Streaming PCM16 to un-normalized log-mel converter.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_MEL_MEL_H
#define ASR_ENGINE_MEL_MEL_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "mel_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Result of a converter operation. */
typedef enum
{
    MEL_OK = 0, /**< The operation completed. */
    MEL_ERROR   /**< Invalid argument, or the converter is not initialized. */
} mel_status_t;

/**
 * @brief Receives one finished feature vector, in time order.
 * @param[in] bands @c ASR_MODEL_MEL_BANDS log-mel values, valid only during the call.
 * @param[in,out] context Argument passed to mel_process().
 */
typedef void (*mel_frame_callback_t)(const float *bands, void *context);

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Build the window, filter bank and twiddle tables, then clear the stream.
 * @retval MEL_OK The converter is ready.
 * @retval MEL_ERROR The filter bank does not fit @c MEL_FILTER_WEIGHT_CAPACITY.
 * @pre Call before any other function.
 */
mel_status_t mel_init(void);

/** @brief Drop the carried samples and start a new stream, e.g. after lost audio. */
void mel_reset(void);

/**
 * @brief Convert a block of PCM16 samples and emit every frame it completes.
 * @param[in] pcm Mono samples at @c ASR_MODEL_SAMPLE_RATE_HZ.
 * @param[in] samples Samples in @p pcm; 0 does nothing.
 * @param[in] on_frame Called once per completed frame; not NULL.
 * @param[in,out] context Forwarded to @p on_frame.
 * @retval MEL_OK The block was consumed.
 * @retval MEL_ERROR Invalid argument, or mel_init() has not succeeded.
 * @note An incomplete frame carries over to the next call.
 */
mel_status_t mel_process(const int16_t *pcm,
                         uint32_t samples,
                         mel_frame_callback_t on_frame,
                         void *context);

/**
 * @brief End the stream; one shorter than a frame yields a single zero-padded frame.
 * @param[in] on_frame Called for the padded frame, if any; not NULL.
 * @param[in,out] context Forwarded to @p on_frame.
 * @retval MEL_OK The stream was closed and the converter reset.
 * @retval MEL_ERROR Invalid argument, or mel_init() has not succeeded.
 * @note A tail after real frames is dropped. Continuous capture never calls it.
 */
mel_status_t mel_flush(mel_frame_callback_t on_frame, void *context);

/**
 * @brief Return how many samples are carried over toward the next frame.
 * @return Buffered samples, from 0 to @c ASR_MODEL_MEL_FRAME_SAMPLES - 1.
 */
uint32_t mel_pending_samples(void);

/**
 * @brief Convert one pre-emphasized frame to log-mel, outside the stream.
 * @param[in] frame @c ASR_MODEL_MEL_FRAME_SAMPLES samples, scaled and pre-emphasized.
 * @param[out] bands @c ASR_MODEL_MEL_BANDS log-mel values.
 * @retval MEL_OK The frame was converted.
 * @retval MEL_ERROR Invalid argument, or mel_init() has not succeeded.
 */
mel_status_t mel_frame_to_logmel(const float *frame, float *bands);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_MEL_MEL_H */
