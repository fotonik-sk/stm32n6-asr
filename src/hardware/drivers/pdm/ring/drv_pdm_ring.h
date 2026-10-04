/**
 * @file     drv_pdm_ring.h
 * @brief    Chunked ring buffer over the microphone DMA capture area.
 * @details  The reader side is the public drv_pdm.h API (drv_pdm_read(),
 *           drv_pdm_available_chunks(), ...), implemented in drv_pdm_ring.c.
 *           This header holds only what the driver itself calls.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_DRIVERS_PDM_RING_DRV_PDM_RING_H
#define HARDWARE_DRIVERS_PDM_RING_DRV_PDM_RING_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "drv_pdm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Geometry of the capture area that the DMA controller fills. */
typedef struct
{
    int16_t *buffer;         /**< First PCM16 sample of the DMA capture area. */
    uint32_t buffer_samples; /**< Capacity of @p buffer in PCM16 samples. */
    uint32_t chunk_samples;  /**< PCM16 samples the DMA writes between two chunk interrupts. */
    uint32_t chunk_count;    /**< Number of chunks the ring holds. */
} drv_pdm_ring_config_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Bind the ring buffer bookkeeping to a capture area.
 * @param[in] config Geometry of the area the DMA writes into. The buffer must
 *                   hold at least @c chunk_samples * @c chunk_count samples and
 *                   must be at least two chunks long.
 * @return DRV_PDM_OK on success, DRV_PDM_ERROR when the geometry is rejected.
 * @pre The DMA is stopped.
 * @note The ring does not own the memory. Place the buffer in the
 *       @c .noncacheable section so the CPU observes the DMA writes.
 */
drv_pdm_status_t drv_pdm_ring_init(const drv_pdm_ring_config_t *config);

/**
 * @brief Drop every stored chunk and clear the dropped-chunk counter.
 * @pre drv_pdm_ring_init() succeeded.
 */
void drv_pdm_ring_reset(void);

/**
 * @brief Publish the chunk the DMA has just finished writing.
 * @pre Called from the DMA chunk-complete interrupt only.
 * @note When the ring is full the oldest chunk is dropped and counted, so
 *       acquisition never stalls.
 */
void drv_pdm_ring_produce(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_DRIVERS_PDM_RING_DRV_PDM_RING_H */
