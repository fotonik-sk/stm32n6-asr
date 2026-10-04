/**
 * @file     mel_ring.c
 * @brief    Single-producer ring buffer bookkeeping for log-mel frames.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mel_ring.h"

#include <stddef.h>
#include <string.h>

/* CMSIS-Core only (PRIMASK intrinsics), no vendor HAL. */
#include "cmsis_compiler.h"

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Storage of the ring, one row per frame. */
static float ring_frames[ASR_CONF_FEATURE_RING_FRAMES][ASR_MODEL_MEL_BANDS];

/** @brief Frames stored since reset; free-running, only @c written - read matters. */
static volatile uint32_t written;

/** @brief Frames the consumer has released since the last reset. */
static volatile uint32_t read;

/** @brief Frames dropped because the consumer had not taken them yet. */
static volatile uint32_t dropped_frames;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/* Every critical section saves PRIMASK, masks interrupts and restores it. */

void mel_ring_reset(void)
{
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    read = written;
    dropped_frames = 0U;
    __set_PRIMASK(primask);
}

void mel_ring_write(const float *bands)
{
    uint32_t primask;

    if (bands == NULL)
    {
        return;
    }

    /* Only the producer advances written, so the slot is filled unlocked and
       published by the increment below. */
    (void)memcpy(ring_frames[written % ASR_CONF_FEATURE_RING_FRAMES],
                 bands,
                 ASR_MODEL_MEL_BANDS * sizeof(float));

    primask = __get_PRIMASK();
    __disable_irq();
    written++;
    if ((written - read) >= ASR_CONF_FEATURE_RING_FRAMES)
    {
        /* Full: drop the oldest frame rather than stall the front-end. */
        read = written - (ASR_CONF_FEATURE_RING_FRAMES - 1U);
        dropped_frames++;
    }
    __set_PRIMASK(primask);
}

uint32_t mel_ring_available(void)
{
    uint32_t primask = __get_PRIMASK();
    uint32_t available;

    __disable_irq();
    available = written - read;
    __set_PRIMASK(primask);
    return available;
}

mel_ring_status_t mel_ring_read(float *destination, uint32_t destination_values, uint32_t frames)
{
    uint32_t primask;
    uint32_t first_frame;
    uint32_t available;
    uint32_t slot;
    uint32_t head_frames;
    mel_ring_status_t status;

    if ((destination == NULL) || (frames == 0U) || (frames >= ASR_CONF_FEATURE_RING_FRAMES))
    {
        return MEL_RING_ERROR;
    }
    if ((destination_values / ASR_MODEL_MEL_BANDS) < frames)
    {
        return MEL_RING_ERROR;
    }

    primask = __get_PRIMASK();
    __disable_irq();
    first_frame = read;
    available = written - first_frame;
    __set_PRIMASK(primask);

    if (available < frames)
    {
        return MEL_RING_UNDERRUN;
    }

    /* Copy unlocked, split at the wrap point. */
    slot = first_frame % ASR_CONF_FEATURE_RING_FRAMES;
    head_frames = ASR_CONF_FEATURE_RING_FRAMES - slot;
    if (head_frames > frames)
    {
        head_frames = frames;
    }
    (void)memcpy(destination,
                 ring_frames[slot],
                 (size_t)head_frames * ASR_MODEL_MEL_BANDS * sizeof(float));
    if (frames > head_frames)
    {
        (void)memcpy(&destination[head_frames * ASR_MODEL_MEL_BANDS],
                     ring_frames[0],
                     (size_t)(frames - head_frames) * ASR_MODEL_MEL_BANDS * sizeof(float));
    }

    /* The copy is valid only if the producer stayed less than one lap ahead. */
    primask = __get_PRIMASK();
    __disable_irq();
    if ((written - first_frame) >= ASR_CONF_FEATURE_RING_FRAMES)
    {
        status = MEL_RING_OVERRUN;
    }
    else
    {
        read = first_frame + frames;
        status = MEL_RING_OK;
    }
    __set_PRIMASK(primask);
    return status;
}

mel_ring_status_t mel_ring_skip(uint32_t frames)
{
    uint32_t primask;
    mel_ring_status_t status;

    if ((frames == 0U) || (frames > ASR_CONF_FEATURE_RING_FRAMES))
    {
        return MEL_RING_ERROR;
    }

    primask = __get_PRIMASK();
    __disable_irq();
    if ((written - read) < frames)
    {
        status = MEL_RING_UNDERRUN;
    }
    else
    {
        read += frames;
        status = MEL_RING_OK;
    }
    __set_PRIMASK(primask);
    return status;
}

uint32_t mel_ring_take_dropped(void)
{
    uint32_t primask = __get_PRIMASK();
    uint32_t dropped;

    __disable_irq();
    dropped = dropped_frames;
    dropped_frames = 0U;
    __set_PRIMASK(primask);
    return dropped;
}
