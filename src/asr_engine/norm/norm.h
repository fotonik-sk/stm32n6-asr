/**
 * @file     norm.h
 * @brief    Streaming normalization of log-mel frames into the network input.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_NORM_NORM_H
#define ASR_ENGINE_NORM_NORM_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "norm_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Result of a normalization operation. */
typedef enum
{
    NORM_OK = 0,       /**< The operation completed; no new window. */
    NORM_WINDOW_READY, /**< The frame closed a block: the input holds a new window. */
    NORM_ERROR         /**< Invalid argument, or the module is not initialized. */
} norm_status_t;

/** @brief Per-band sums of one closed block. */
typedef struct
{
    double sum[ASR_MODEL_MEL_BANDS];         /**< Sum of the block's values. */
    double sum_squares[ASR_MODEL_MEL_BANDS]; /**< Sum of their squares. */
} norm_block_stats_t;

/** @brief Caller-owned memory of the stage; the three regions must not overlap. */
typedef struct
{
    /** @brief @c NORM_FIELD_VALUES int8 codes, oldest frame first; may be the network input. */
    int8_t *input;

    /** @brief @c NORM_FIELD_VALUES floats: the raw receptive field ring, live for the stream. */
    float *field;

    /** @brief @c NORM_STATS_BLOCKS block sums: the statistics ring, live for the stream. */
    norm_block_stats_t *stats;
} norm_buffers_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Bind the caller's buffers and start a new stream.
 * @param[in] buffers The three buffers; the pointers are copied.
 * @retval NORM_OK Ready for norm_push_frame().
 * @retval NORM_ERROR A pointer is NULL, or two buffers overlap.
 */
norm_status_t norm_init(const norm_buffers_t *buffers);

/** @brief Start a new stream: the field back to silence, the statistics empty. */
void norm_reset(void);

/**
 * @brief Add one log-mel frame; renormalize the field when it closes a block.
 * @param[in] bands @c ASR_MODEL_MEL_BANDS raw log-mel values; copied.
 * @retval NORM_OK The frame was stored.
 * @retval NORM_WINDOW_READY The input now holds the newest
 *         @c ASR_NETWORK_INPUT_FRAMES frames as int8 codes.
 * @retval NORM_ERROR @p bands is NULL, or norm_init() has not succeeded.
 */
norm_status_t norm_push_frame(const float *bands);

/**
 * @brief Tell whether the next norm_push_frame() overwrites the input buffer.
 * @return Non-zero when the next frame closes a block.
 */
int norm_next_frame_writes_window(void);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_NORM_NORM_H */
