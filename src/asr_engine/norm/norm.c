/**
 * @file     norm.c
 * @brief    Receptive-field normalization and int8 quantization; implements norm.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "norm.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

/*******************************************************************************/
/*                              STATIC ASSERTIONS                              */
/*******************************************************************************/

_Static_assert(ASR_MODEL_NET_STRIDE_FRAMES > 0U, "a block needs at least one frame");
_Static_assert((ASR_NETWORK_INPUT_FRAMES % ASR_MODEL_NET_STRIDE_FRAMES) == 0U,
               "the receptive field must be a whole number of blocks: the field "
               "ring wraps on a block boundary and windows are cut there");
_Static_assert((ASR_MODEL_NORM_STATS_FRAMES % ASR_MODEL_NET_STRIDE_FRAMES) == 0U,
               "the statistics memory is kept as one accumulator per block, so it "
               "must be a whole number of blocks");
_Static_assert(ASR_MODEL_NORM_STATS_FRAMES >= ASR_NETWORK_INPUT_FRAMES,
               "statistics over less than the receptive field would normalize a "
               "window with numbers taken from a part of it");
_Static_assert((ASR_NETWORK_INPUT_ZERO_POINT >= -128) && (ASR_NETWORK_INPUT_ZERO_POINT <= 127),
               "the zero point must be an int8 code");

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief Lowest int8 code, as the float the quantizer clamps to. */
#define NORM_CODE_MIN (-128.0f)

/** @brief Highest int8 code, as the float the quantizer clamps to. */
#define NORM_CODE_MAX 127.0f

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Mean of the statistics memory, per band. */
static double band_mean[ASR_MODEL_MEL_BANDS];

/** @brief Deviation of the statistics memory, per band, never below ASR_MODEL_NORM_SIGMA_FLOOR. */
static double band_deviation[ASR_MODEL_MEL_BANDS];

/** @brief Caller's network input window, @c NORM_FIELD_VALUES codes. */
static int8_t *input;

/** @brief Caller's raw receptive field, @c NORM_FIELD_VALUES floats, as a ring. */
static float *field;

/** @brief Caller's statistics memory, @c NORM_STATS_BLOCKS accumulators, as a ring. */
static norm_block_stats_t *stats;

/** @brief Frame the next push writes in the field ring; also its oldest frame once full. */
static uint32_t field_write;

/** @brief Frames already added to the open block. */
static uint32_t frame_in_block;

/** @brief Statistics slot the open block accumulates into. */
static uint32_t stats_slot;

/** @brief Slots of the statistics ring that hold a closed block, saturating at NORM_STATS_BLOCKS. */
static uint32_t stats_blocks;

/** @brief Set once norm_init() has bound the buffers. */
static int ready;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Tell whether two memory regions share at least one byte.
 * @param[in] first Start of the first region.
 * @param[in] first_bytes Size of the first region.
 * @param[in] second Start of the second region.
 * @param[in] second_bytes Size of the second region.
 * @return Non-zero when the regions overlap.
 */
static int regions_overlap(const void *first, size_t first_bytes,
                           const void *second, size_t second_bytes)
{
    uintptr_t first_start = (uintptr_t)first;
    uintptr_t second_start = (uintptr_t)second;

    return (first_start < (second_start + second_bytes)) &&
           (second_start < (first_start + first_bytes));
}

/**
 * @brief Derive the mean and the deviation from every block the statistics ring holds.
 * @note Filled slots are always @c 0 .. @c stats_blocks-1; a sum ignores their order.
 */
static void update_statistics(void)
{
    const uint32_t frames = stats_blocks * ASR_MODEL_NET_STRIDE_FRAMES;
    uint32_t band;

    for (band = 0U; band < ASR_MODEL_MEL_BANDS; ++band)
    {
        double sum = 0.0;
        double sum_squares = 0.0;
        double mean;
        double variance;
        double deviation;
        uint32_t slot;

        for (slot = 0U; slot < stats_blocks; ++slot)
        {
            sum += stats[slot].sum[band];
            sum_squares += stats[slot].sum_squares[band];
        }

        /* Population variance from the sum of squares. */
        mean = sum / (double)frames;
        variance = (sum_squares / (double)frames) - (mean * mean);
        if (variance < 0.0)
        {
            variance = 0.0;
        }
        deviation = sqrt(variance);
        if (deviation < ASR_MODEL_NORM_SIGMA_FLOOR)
        {
            deviation = ASR_MODEL_NORM_SIGMA_FLOOR;
        }
        band_mean[band] = mean;
        band_deviation[band] = deviation;
    }
}

/**
 * @brief Normalize and quantize frames with the current statistics.
 * @param[in] source @c frames * ASR_MODEL_MEL_BANDS raw log-mel values.
 * @param[out] destination As many int8 codes.
 * @param[in] frames Frames to convert.
 */
static void normalize_frames(const float *source, int8_t *destination, uint32_t frames)
{
    uint32_t frame;

    for (frame = 0U; frame < frames; ++frame)
    {
        uint32_t band;

        for (band = 0U; band < ASR_MODEL_MEL_BANDS; ++band)
        {
            float normalized = (float)(((double)*source - band_mean[band]) / band_deviation[band]);
            float code = (normalized / ASR_NETWORK_INPUT_SCALE) + (float)ASR_NETWORK_INPUT_ZERO_POINT;

            /* Clamp to the int8 range; keeps lrintf() within long. */
            if (code < NORM_CODE_MIN)
            {
                code = NORM_CODE_MIN;
            }
            else if (code > NORM_CODE_MAX)
            {
                code = NORM_CODE_MAX;
            }
            /* Round half to even. */
            *destination = (int8_t)lrintf(code);

            ++source;
            ++destination;
        }
    }
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

norm_status_t norm_init(const norm_buffers_t *buffers)
{
    const size_t input_bytes = NORM_FIELD_VALUES * sizeof(int8_t);
    const size_t field_bytes = NORM_FIELD_VALUES * sizeof(float);
    const size_t stats_bytes = NORM_STATS_BLOCKS * sizeof(norm_block_stats_t);

    if ((buffers == NULL) || (buffers->input == NULL) ||
        (buffers->field == NULL) || (buffers->stats == NULL))
    {
        return NORM_ERROR;
    }
    if (regions_overlap(buffers->input, input_bytes, buffers->field, field_bytes) ||
        regions_overlap(buffers->input, input_bytes, buffers->stats, stats_bytes) ||
        regions_overlap(buffers->field, field_bytes, buffers->stats, stats_bytes))
    {
        return NORM_ERROR;
    }

    input = buffers->input;
    field = buffers->field;
    stats = buffers->stats;
    ready = 1;
    norm_reset();
    return NORM_OK;
}

void norm_reset(void)
{
    if (field != NULL)
    {
        const float silence = logf(ASR_MODEL_MEL_LOG_ZERO_GUARD);
        uint32_t value;

        /* Frames not received yet are digital silence, kept out of the statistics. */
        for (value = 0U; value < NORM_FIELD_VALUES; ++value)
        {
            field[value] = silence;
        }
    }
    field_write = 0U;
    frame_in_block = 0U;
    stats_slot = 0U;
    stats_blocks = 0U;
}

norm_status_t norm_push_frame(const float *bands)
{
    float *destination;
    uint32_t band;
    uint32_t tail_frames;

    if (!ready || (bands == NULL))
    {
        return NORM_ERROR;
    }

    if (frame_in_block == 0U)
    {
        /* A full ring reuses this slot: drop its oldest block. */
        (void)memset(&stats[stats_slot], 0, sizeof(stats[0]));
    }

    destination = &field[field_write * ASR_MODEL_MEL_BANDS];
    for (band = 0U; band < ASR_MODEL_MEL_BANDS; ++band)
    {
        double value = (double)bands[band];

        destination[band] = bands[band];
        stats[stats_slot].sum[band] += value;
        stats[stats_slot].sum_squares[band] += value * value;
    }

    ++field_write;
    if (field_write == ASR_NETWORK_INPUT_FRAMES)
    {
        field_write = 0U;
    }
    ++frame_in_block;
    if (frame_in_block < ASR_MODEL_NET_STRIDE_FRAMES)
    {
        return NORM_OK;
    }
    frame_in_block = 0U;

    /* The closed block joins the statistics; a full ring stops growing. */
    if (stats_blocks < NORM_STATS_BLOCKS)
    {
        ++stats_blocks;
    }
    update_statistics();
    ++stats_slot;
    if (stats_slot == NORM_STATS_BLOCKS)
    {
        stats_slot = 0U;
    }

    /* Normalize the whole field into the input, oldest frame first. The oldest
       frame of the ring is at field_write, so it takes two runs at most. */
    tail_frames = ASR_NETWORK_INPUT_FRAMES - field_write;
    normalize_frames(&field[field_write * ASR_MODEL_MEL_BANDS], input, tail_frames);
    if (field_write != 0U)
    {
        normalize_frames(field, &input[tail_frames * ASR_MODEL_MEL_BANDS], field_write);
    }
    return NORM_WINDOW_READY;
}

int norm_next_frame_writes_window(void)
{
    return ready && (frame_in_block == (ASR_MODEL_NET_STRIDE_FRAMES - 1U));
}
