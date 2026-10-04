/**
 * @file     drv_pdm_dma_config.h
 * @brief    Compile-time size of the microphone capture area.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_DRIVERS_PDM_DMA_DRV_PDM_DMA_CONFIG_H
#define HARDWARE_DRIVERS_PDM_DMA_DRV_PDM_DMA_CONFIG_H

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/**
 * @brief PCM16 capacity of the capture area the DMA module owns.
 * @note 1600 samples is 100 ms at the 16 kHz the MDF chain produces. Any chunk
 *       geometry fits as long as @c chunk_samples * @c chunk_count stays within
 *       this budget, so 10 chunks of 10 ms and 5 chunks of 20 ms are both valid.
 *       The area is a static array in the non-cacheable section and costs
 *       @c 2 * DRV_PDM_DMA_CAPTURE_SAMPLES bytes of the image.
 */
#ifndef DRV_PDM_DMA_CAPTURE_SAMPLES
#define DRV_PDM_DMA_CAPTURE_SAMPLES 1600U
#endif

/**
 * @brief Largest chunk count drv_pdm_dma_configure() accepts.
 * @note One linked-list node is reserved per chunk, so this sizes the node pool
 *       rather than the capture area.
 */
#ifndef DRV_PDM_DMA_MAX_CHUNKS
#define DRV_PDM_DMA_MAX_CHUNKS 16U
#endif

#endif /* HARDWARE_DRIVERS_PDM_DMA_DRV_PDM_DMA_CONFIG_H */
