/**
 * @file     drv_pdm.c
 * @brief    Start-up sequencing of the PDM filter, its capture DMA and the ring
 *           buffer: drv_pdm_init() and drv_pdm_stop(). The ring implements the
 *           reader side of drv_pdm.h, the DMA module its interrupt dispatch.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "drv_pdm.h"

#include <stdbool.h>

#include "drv_pdm_dma.h"
#include "drv_pdm_dma_config.h"
#include "drv_pdm_mdf.h"
#include "drv_pdm_ring.h"

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Chunk notification requested by the caller of drv_pdm_init(). */
static volatile drv_pdm_chunk_callback_t chunk_callback;

/** @brief Argument passed back to chunk_callback. */
static void *callback_context;

/** @brief Set once acquisition is configured and running. */
static bool running;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Publish the completed chunk and forward the notification.
 * @note Runs in GPDMA1 channel 6 interrupt context, once per chunk.
 */
static void on_chunk_complete(void)
{
    drv_pdm_chunk_callback_t callback = chunk_callback;

    drv_pdm_ring_produce();
    if (callback != NULL)
    {
        callback(callback_context);
    }
}

/**
 * @brief Configure the DMA, the ring buffer and the PDM filter, in that order.
 * @param[in] config Capture geometry and board hooks supplied by the caller.
 * @return DRV_PDM_OK on success, DRV_PDM_ERROR when a configuration step fails.
 * @note Nothing is started yet; drv_pdm_init() starts the acquisition.
 */
static drv_pdm_status_t configure_hardware(const drv_pdm_config_t *config)
{
    drv_pdm_dma_config_t dma_config = {
        .chunk_samples = config->chunk_samples,
        .chunk_count = config->chunk_count,
        .irq_priority = config->irq_priority,
    };
    drv_pdm_ring_config_t ring_config = {
        .buffer = drv_pdm_dma_buffer(),
        .buffer_samples = DRV_PDM_DMA_CAPTURE_SAMPLES,
        .chunk_samples = config->chunk_samples,
        .chunk_count = config->chunk_count,
    };

    /* 1. DMA geometry first: HAL_MDF_Init() below builds the capture queue from it. */
    if (drv_pdm_dma_configure(&dma_config) != HAL_OK)
    {
        return DRV_PDM_ERROR;
    }
    /* 2. The ring over the same capture area, empty. */
    if (drv_pdm_ring_init(&ring_config) != DRV_PDM_OK)
    {
        return DRV_PDM_ERROR;
    }
    /* 3. The filter. Its MSP hook clocks MDF1, routes the pins and links the DMA
          queue; a failure inside the hook shows only in the error flags. */
    if ((drv_pdm_mdf_init(config->init_pins) != HAL_OK) || (drv_pdm_dma_has_error() != 0U))
    {
        return DRV_PDM_ERROR;
    }
    /* 4. The channel exists now, so its security attributes can be set. */
    if (drv_pdm_dma_configure_security() != HAL_OK)
    {
        return DRV_PDM_ERROR;
    }
    return DRV_PDM_OK;
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

drv_pdm_status_t drv_pdm_init(const drv_pdm_config_t *config)
{
    if (running)
    {
        return DRV_PDM_OK;
    }
    if ((config == NULL) || (config->init_pins == NULL))
    {
        return DRV_PDM_ERROR;
    }

    /* Every completed chunk goes DMA interrupt -> on_chunk_complete() -> ring
       + the caller's callback. */
    chunk_callback = config->chunk_callback;
    callback_context = config->callback_context;
    drv_pdm_dma_set_chunk_callback(on_chunk_complete);

    if (configure_hardware(config) != DRV_PDM_OK)
    {
        drv_pdm_dma_set_chunk_callback(NULL);
        return DRV_PDM_ERROR;
    }

    /* The start sequence rewrites the head node with the start of the capture
       area and one chunk's length: the values the queue already holds. */
    drv_pdm_dma_enable_interrupt();
    if (drv_pdm_mdf_start((uint32_t)drv_pdm_dma_buffer(), drv_pdm_chunk_bytes()) != HAL_OK)
    {
        drv_pdm_dma_disable_interrupt();
        drv_pdm_dma_set_chunk_callback(NULL);
        return DRV_PDM_ERROR;
    }
    /* Chunks are published whole, so the mid-chunk event is pure overhead. */
    drv_pdm_dma_suppress_half_transfer();

    running = true;
    return DRV_PDM_OK;
}

drv_pdm_status_t drv_pdm_stop(void)
{
    running = false;
    drv_pdm_dma_disable_interrupt();
    return (drv_pdm_mdf_stop() == HAL_OK) ? DRV_PDM_OK : DRV_PDM_ERROR;
}
