/**
 * @file     drv_pdm_ring.c
 * @brief    Chunked ring buffer bookkeeping for the microphone DMA capture area;
 *           implements drv_pdm_ring.h and the reader side of drv_pdm.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "drv_pdm_ring.h"

#include <stddef.h>
#include <string.h>

#include "stm32n6xx_hal.h"

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Capture area the DMA writes into, owned by the caller of drv_pdm_ring_init(). */
static int16_t *buffer;

/** @brief PCM16 samples the DMA writes between two chunk interrupts. */
static uint32_t chunk_samples;

/** @brief Number of chunks the ring holds. */
static uint32_t chunk_count;

/**
 * @brief Chunks the DMA has completed since the last reset.
 * @note Free-running; only differences against read are meaningful.
 */
static volatile uint32_t written;

/** @brief Chunks the reader has released since the last reset. */
static volatile uint32_t read;

/** @brief Chunks the DMA dropped because the reader had not taken them yet. */
static volatile uint32_t dropped_chunks;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/* Every critical section saves PRIMASK, masks interrupts and restores it. */

drv_pdm_status_t drv_pdm_ring_init(const drv_pdm_ring_config_t *config)
{
    uint32_t primask;

    if ((config == NULL) || (config->buffer == NULL) || (config->chunk_samples == 0U) ||
        (config->chunk_count < 2U))
    {
        return DRV_PDM_ERROR;
    }
    if (config->chunk_samples > (config->buffer_samples / config->chunk_count))
    {
        return DRV_PDM_ERROR;
    }

    primask = __get_PRIMASK();
    __disable_irq();
    buffer = config->buffer;
    chunk_samples = config->chunk_samples;
    chunk_count = config->chunk_count;
    written = 0U;
    read = 0U;
    dropped_chunks = 0U;
    __set_PRIMASK(primask);
    return DRV_PDM_OK;
}

void drv_pdm_ring_reset(void)
{
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    read = written;
    dropped_chunks = 0U;
    __set_PRIMASK(primask);
}

void drv_pdm_ring_produce(void)
{
    uint32_t primask;

    if (buffer == NULL)
    {
        return;
    }

    primask = __get_PRIMASK();
    __disable_irq();
    written++;
    if ((written - read) >= chunk_count)
    {
        /* The DMA is about to reuse the slot of the oldest stored chunk: drop it
           and keep acquiring instead of stalling the capture. */
        read = written - (chunk_count - 1U);
        dropped_chunks++;
    }
    __set_PRIMASK(primask);
}

uint32_t drv_pdm_available_chunks(void)
{
    uint32_t primask;
    uint32_t available;

    if (buffer == NULL)
    {
        return 0U;
    }

    primask = __get_PRIMASK();
    __disable_irq();
    available = written - read;
    __set_PRIMASK(primask);
    return available;
}

uint32_t drv_pdm_chunk_samples(void)
{
    return (buffer != NULL) ? chunk_samples : 0U;
}

uint32_t drv_pdm_chunk_bytes(void)
{
    return (buffer != NULL) ? (chunk_samples * (uint32_t)sizeof(int16_t)) : 0U;
}

uint32_t drv_pdm_chunk_capacity(void)
{
    return (buffer != NULL) ? chunk_count : 0U;
}

drv_pdm_status_t drv_pdm_read(int16_t *destination, uint32_t destination_samples, uint32_t chunks)
{
    uint32_t primask;
    uint32_t first_chunk;
    uint32_t available;
    uint32_t slot;
    uint32_t head_chunks;
    drv_pdm_status_t status;

    if ((buffer == NULL) || (destination == NULL) || (chunks == 0U) ||
        (chunks >= chunk_count))
    {
        return DRV_PDM_ERROR;
    }
    if ((destination_samples / chunk_samples) < chunks)
    {
        return DRV_PDM_ERROR;
    }

    primask = __get_PRIMASK();
    __disable_irq();
    first_chunk = read;
    available = written - first_chunk;
    __set_PRIMASK(primask);

    if (available < chunks)
    {
        return DRV_PDM_UNDERRUN;
    }

    /* Copy with the DMA active, split at the wrap point. */
    slot = first_chunk % chunk_count;
    head_chunks = chunk_count - slot;
    if (head_chunks > chunks)
    {
        head_chunks = chunks;
    }
    (void)memcpy(destination,
                 &buffer[slot * chunk_samples],
                 (size_t)head_chunks * chunk_samples * sizeof(int16_t));
    if (chunks > head_chunks)
    {
        (void)memcpy(&destination[head_chunks * chunk_samples],
                     buffer,
                     (size_t)(chunks - head_chunks) * chunk_samples * sizeof(int16_t));
    }

    /* The oldest copied chunk survives only while the writer stays less than
       one full lap ahead of first_chunk. */
    primask = __get_PRIMASK();
    __disable_irq();
    if ((written - first_chunk) >= chunk_count)
    {
        status = DRV_PDM_OVERRUN;
    }
    else
    {
        read = first_chunk + chunks;
        status = DRV_PDM_OK;
    }
    __set_PRIMASK(primask);
    return status;
}

drv_pdm_status_t drv_pdm_skip(uint32_t chunks)
{
    uint32_t primask;
    drv_pdm_status_t status;

    if ((buffer == NULL) || (chunks == 0U) || (chunks > chunk_count))
    {
        return DRV_PDM_ERROR;
    }

    primask = __get_PRIMASK();
    __disable_irq();
    if ((written - read) < chunks)
    {
        status = DRV_PDM_UNDERRUN;
    }
    else
    {
        read += chunks;
        status = DRV_PDM_OK;
    }
    __set_PRIMASK(primask);
    return status;
}

uint32_t drv_pdm_take_dropped_chunks(void)
{
    uint32_t primask = __get_PRIMASK();
    uint32_t dropped;

    __disable_irq();
    dropped = dropped_chunks;
    dropped_chunks = 0U;
    __set_PRIMASK(primask);
    return dropped;
}
