/**
 * @file     mel_ring.h
 * @brief    Single-producer ring of finished log-mel feature vectors.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_MEL_MEL_RING_H
#define ASR_ENGINE_MEL_MEL_RING_H

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

/** @brief Result of a feature ring operation. */
typedef enum
{
    MEL_RING_OK = 0,   /**< The operation completed. */
    MEL_RING_ERROR,    /**< Invalid argument. */
    MEL_RING_UNDERRUN, /**< Fewer frames are stored than were requested. */
    MEL_RING_OVERRUN   /**< The producer reached the frames while they were copied. */
} mel_ring_status_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/** @brief Drop every stored frame and clear the dropped-frame counter. */
void mel_ring_reset(void);

/**
 * @brief Append one feature vector; when full, drop and count the oldest one.
 * @param[in] bands @c ASR_MODEL_MEL_BANDS log-mel values; copied.
 */
void mel_ring_write(const float *bands);

/**
 * @brief Return the number of frames waiting to be read.
 * @return Stored frame count.
 */
uint32_t mel_ring_available(void);

/**
 * @brief Copy the oldest frames out of the ring and release them.
 * @param[out] destination Receives @c frames * ASR_MODEL_MEL_BANDS values, oldest first.
 * @param[in] destination_values Capacity of @p destination in floats.
 * @param[in] frames Frames to copy, 1 .. @c ASR_CONF_FEATURE_RING_FRAMES - 1 (one
 *            slot is kept for the producer).
 * @retval MEL_RING_OK Every frame was copied and released.
 * @retval MEL_RING_ERROR Invalid argument, or @p destination is too small.
 * @retval MEL_RING_UNDERRUN Fewer frames are stored; nothing was copied.
 * @retval MEL_RING_OVERRUN The producer overwrote them during the copy; the
 *         data is discarded and reading resumes at the newest frames.
 * @pre Single consumer thread. The copy runs with interrupts enabled.
 */
mel_ring_status_t mel_ring_read(float *destination, uint32_t destination_values, uint32_t frames);

/**
 * @brief Release the oldest frames without copying them.
 * @param[in] frames Frames to drop.
 * @retval MEL_RING_OK The frames were released.
 * @retval MEL_RING_ERROR Invalid argument.
 * @retval MEL_RING_UNDERRUN Fewer frames are stored; nothing was released.
 */
mel_ring_status_t mel_ring_skip(uint32_t frames);

/**
 * @brief Read and clear the number of frames dropped because the ring was full.
 * @return Frames overwritten before they were read since the previous call.
 */
uint32_t mel_ring_take_dropped(void);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_MEL_MEL_RING_H */
