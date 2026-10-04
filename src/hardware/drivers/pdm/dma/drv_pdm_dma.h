/**
 * @file     drv_pdm_dma.h
 * @brief    GPDMA1 channel 6 circular capture of the MDF1 filter output.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_DRIVERS_PDM_DMA_DRV_PDM_DMA_H
#define HARDWARE_DRIVERS_PDM_DMA_DRV_PDM_DMA_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "drv_pdm_dma_config.h"
#include "stm32n6xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Notification raised once per completed chunk, in interrupt context. */
typedef void (*drv_pdm_dma_chunk_callback_t)(void);

/** @brief Chunk geometry carved out of the capture area this module owns. */
typedef struct
{
    uint32_t chunk_samples; /**< PCM16 samples one node transfers. */
    uint32_t chunk_count;   /**< Nodes in the circular queue, from 2 to DRV_PDM_DMA_MAX_CHUNKS. */
    uint32_t irq_priority;  /**< GPDMA1 channel 6 preemption priority. */
} drv_pdm_dma_config_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Validate the chunk geometry and enable the GPDMA1 clock.
 * @param[in] config Chunk geometry to carve out of the capture area.
 * @return HAL_OK when @c chunk_samples * @c chunk_count fits
 *         DRV_PDM_DMA_CAPTURE_SAMPLES and the chunk count fits the node pool,
 *         HAL_ERROR otherwise.
 * @pre The channel is stopped. Call before drv_pdm_mdf_init(), which triggers
 *      drv_pdm_dma_link() from the MDF MSP hook.
 */
HAL_StatusTypeDef drv_pdm_dma_configure(const drv_pdm_dma_config_t *config);

/**
 * @brief Return the capture area the linked-list queue writes into.
 * @return First PCM16 sample of the area, which always holds
 *         DRV_PDM_DMA_CAPTURE_SAMPLES samples regardless of the active geometry.
 * @note The module owns this memory and places it in the non-cacheable section,
 *       so callers never have to arrange for cache maintenance.
 */
int16_t *drv_pdm_dma_buffer(void);

/**
 * @brief Build the circular linked-list queue and attach it to the MDF handle.
 * @param[in,out] hmdf MDF handle that receives the DMA handle through __HAL_LINKDMA().
 * @return HAL_OK on success, HAL_ERROR when a HAL list operation fails.
 * @pre drv_pdm_dma_configure() succeeded.
 * @note Called from HAL_MDF_MspInit(). One node per chunk raises one
 *       transfer-complete event per chunk, and the last node loops to the first.
 */
HAL_StatusTypeDef drv_pdm_dma_link(MDF_HandleTypeDef *hmdf);

/**
 * @brief Detach the queue and release the GPDMA1 channel.
 * @param[in,out] hmdf MDF handle whose DMA handle is de-initialized.
 * @note Called from HAL_MDF_MspDeInit().
 */
void drv_pdm_dma_unlink(MDF_HandleTypeDef *hmdf);

/**
 * @brief Mark the channel and both of its ports as secure and privileged.
 * @return HAL_OK on success, HAL_ERROR when the RIFSC update fails.
 * @pre drv_pdm_dma_link() succeeded.
 */
HAL_StatusTypeDef drv_pdm_dma_configure_security(void);

/**
 * @brief Enable the GPDMA1 channel 6 interrupt at the configured priority.
 * @pre drv_pdm_dma_configure() succeeded.
 */
void drv_pdm_dma_enable_interrupt(void);

/**
 * @brief Disable the GPDMA1 channel 6 interrupt.
 */
void drv_pdm_dma_disable_interrupt(void);

/**
 * @brief Mask the half-transfer interrupt left enabled by the MDF start sequence.
 * @pre The acquisition has been started.
 * @note Only whole chunks are published, so the mid-chunk event is pure overhead.
 */
void drv_pdm_dma_suppress_half_transfer(void);

/**
 * @brief Register the notification raised once per completed chunk.
 * @param[in] callback Function called from interrupt context, or NULL to detach.
 */
void drv_pdm_dma_set_chunk_callback(drv_pdm_dma_chunk_callback_t callback);

/**
 * @brief Report whether the MSP hooks recorded a hardware configuration failure.
 * @return Non-zero after a failed clock, GPIO or linked-list operation.
 */
uint8_t drv_pdm_dma_has_error(void);

/* drv_pdm_irq_handler() of drv_pdm.h is implemented in drv_pdm_dma.c. */

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_DRIVERS_PDM_DMA_DRV_PDM_DMA_H */
