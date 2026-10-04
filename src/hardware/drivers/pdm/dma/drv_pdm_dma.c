/**
 * @file     drv_pdm_dma.c
 * @brief    GPDMA1 channel 6 circular linked-list capture of MDF1 filter 0.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "drv_pdm_dma.h"

#include "drv_pdm.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief Largest block size the channel block register can express, in PCM16 samples. */
#define DRV_PDM_DMA_BLOCK_LIMIT_SAMPLES 32767U

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/**
 * @brief Linked-list nodes fetched by the DMA controller, one per chunk.
 * @note The controller reads them without going through the D-cache.
 */
static DMA_NodeTypeDef nodes[DRV_PDM_DMA_MAX_CHUNKS] __NON_CACHEABLE;

/**
 * @brief Capture area the linked-list queue writes into.
 * @note Owned by this module so its non-cacheable placement cannot be got wrong
 *       by a caller. The DMA writes it without going through the D-cache.
 */
static int16_t capture[DRV_PDM_DMA_CAPTURE_SAMPLES] __NON_CACHEABLE;

/** @brief Circular queue holding the active nodes. */
static DMA_QListTypeDef queue;

/** @brief HAL handle of GPDMA1 channel 6. */
static DMA_HandleTypeDef dma;

/** @brief Chunk geometry accepted by drv_pdm_dma_configure(). */
static drv_pdm_dma_config_t active_config;

/** @brief Bytes one node transfers, derived from the active geometry. */
static uint32_t chunk_bytes;

/** @brief Notification raised once per completed chunk. */
static volatile drv_pdm_dma_chunk_callback_t chunk_callback;

/** @brief Set when a hardware configuration step failed. */
static volatile uint8_t error;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Fill the node template shared by every chunk of the capture queue.
 * @param[out] node_config Node configuration to initialize.
 * @details Every field mirrors the MDF1 filter 0 capture path: half-word
 *          peripheral-to-memory transfers from a fixed source, one block
 *          transfer event per node, and secure source and destination ports.
 */
static void build_node_template(DMA_NodeConfTypeDef *node_config)
{
    node_config->NodeType = DMA_GPDMA_LINEAR_NODE;
    node_config->Init.Request = GPDMA1_REQUEST_MDF1_FLT0;
    node_config->Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    node_config->Init.Direction = DMA_PERIPH_TO_MEMORY;
    node_config->Init.SrcInc = DMA_SINC_FIXED;
    node_config->Init.DestInc = DMA_DINC_INCREMENTED;
    node_config->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_HALFWORD;
    node_config->Init.DestDataWidth = DMA_DEST_DATAWIDTH_HALFWORD;
    node_config->Init.SrcBurstLength = 1U;
    node_config->Init.DestBurstLength = 1U;
    node_config->Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
    node_config->Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    node_config->Init.Mode = DMA_NORMAL;
    node_config->TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
    node_config->DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
    node_config->DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
    node_config->SrcSecure = DMA_CHANNEL_SRC_SEC;
    node_config->DestSecure = DMA_CHANNEL_DEST_SEC;
}

/**
 * @brief Build the per-chunk nodes and chain them into a circular queue.
 * @return HAL_OK on success, HAL_ERROR when a HAL list operation fails.
 * @note The head node addresses and length are re-applied by
 *       HAL_MDF_AcqStart_DMA(), which writes the same values back.
 */
static HAL_StatusTypeDef build_queue(void)
{
    DMA_NodeConfTypeDef node_config = {0};
    /* DFLTDR holds the sample left-aligned in 32 bits; its upper half-word
       (byte offset 2, little-endian) is the PCM16 sample. */
    uint32_t source = ((uint32_t)&MDF1_Filter0->DFLTDR) + 2U;

    build_node_template(&node_config);
    queue.Head = NULL;
    queue.FirstCircularNode = NULL;
    queue.NodeNumber = 0U;
    queue.State = HAL_DMA_QUEUE_STATE_RESET;
    queue.ErrorCode = 0U;
    queue.Type = 0U;

    for (uint32_t chunk = 0U; chunk < active_config.chunk_count; chunk++)
    {
        node_config.SrcAddress = source;
        node_config.DstAddress = (uint32_t)&capture[chunk * active_config.chunk_samples];
        node_config.DataSize = chunk_bytes;
        if (HAL_DMAEx_List_BuildNode(&node_config, &nodes[chunk]) != HAL_OK)
        {
            return HAL_ERROR;
        }
        if (HAL_DMAEx_List_InsertNode_Tail(&queue, &nodes[chunk]) != HAL_OK)
        {
            return HAL_ERROR;
        }
    }
    return HAL_DMAEx_List_SetCircularMode(&queue);
}

/**
 * @brief Initialize GPDMA1 channel 6 for circular linked-list execution.
 * @return HAL_OK on success, HAL_ERROR when a HAL operation fails.
 */
static HAL_StatusTypeDef init_channel(void)
{
    dma.Instance = GPDMA1_Channel6;
    dma.InitLinkedList.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    dma.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
    dma.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
    dma.InitLinkedList.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    dma.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;
    if (HAL_DMAEx_List_Init(&dma) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_DMAEx_List_LinkQ(&dma, &queue);
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

HAL_StatusTypeDef drv_pdm_dma_configure(const drv_pdm_dma_config_t *config)
{
    error = 0U;

    if ((config == NULL) || (config->chunk_samples == 0U) ||
        (config->chunk_samples > DRV_PDM_DMA_BLOCK_LIMIT_SAMPLES))
    {
        return HAL_ERROR;
    }
    if ((config->chunk_count < 2U) || (config->chunk_count > DRV_PDM_DMA_MAX_CHUNKS))
    {
        return HAL_ERROR;
    }
    /* Divide rather than multiply so the check cannot overflow. */
    if (config->chunk_samples > (DRV_PDM_DMA_CAPTURE_SAMPLES / config->chunk_count))
    {
        return HAL_ERROR;
    }
    /* CLBAR holds one base address for the whole queue, so every node must sit
       in the same 64 KB region. */
    if (((uint32_t)&nodes[0] & 0xFFFF0000U) !=
        ((uint32_t)&nodes[config->chunk_count - 1U] & 0xFFFF0000U))
    {
        return HAL_ERROR;
    }

    active_config = *config;
    chunk_bytes = config->chunk_samples * (uint32_t)sizeof(int16_t);
    __HAL_RCC_GPDMA1_CLK_ENABLE();
    return HAL_OK;
}

int16_t *drv_pdm_dma_buffer(void)
{
    return capture;
}

HAL_StatusTypeDef drv_pdm_dma_link(MDF_HandleTypeDef *hmdf)
{
    if ((hmdf == NULL) || (chunk_bytes == 0U))
    {
        error = 1U;
        return HAL_ERROR;
    }
    if ((build_queue() != HAL_OK) || (init_channel() != HAL_OK))
    {
        error = 1U;
        return HAL_ERROR;
    }

    __HAL_LINKDMA(hmdf, hdma, dma);
    return HAL_OK;
}

void drv_pdm_dma_unlink(MDF_HandleTypeDef *hmdf)
{
    if ((hmdf != NULL) && (hmdf->hdma != NULL))
    {
        (void)HAL_DMA_DeInit(hmdf->hdma);
    }
}

HAL_StatusTypeDef drv_pdm_dma_configure_security(void)
{
    __HAL_RCC_RIFSC_CLK_ENABLE();
    return HAL_DMA_ConfigChannelAttributes(&dma,
                                           DMA_CHANNEL_SEC | DMA_CHANNEL_PRIV |
                                               DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_DEST_SEC);
}

void drv_pdm_dma_enable_interrupt(void)
{
    HAL_NVIC_ClearPendingIRQ(GPDMA1_Channel6_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel6_IRQn, active_config.irq_priority, 0U);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel6_IRQn);
}

void drv_pdm_dma_disable_interrupt(void)
{
    HAL_NVIC_DisableIRQ(GPDMA1_Channel6_IRQn);
}

void drv_pdm_dma_suppress_half_transfer(void)
{
    __HAL_DMA_DISABLE_IT(&dma, DMA_IT_HT);
}

void drv_pdm_dma_set_chunk_callback(drv_pdm_dma_chunk_callback_t callback)
{
    chunk_callback = callback;
}

uint8_t drv_pdm_dma_has_error(void)
{
    return error;
}

/* Declared in drv_pdm.h: the channel handle lives here. */
void drv_pdm_irq_handler(void)
{
    HAL_DMA_IRQHandler(&dma);
}

/**
 * @brief Report the end of one linked-list node, that is one complete chunk.
 * @param[in] hmdf MDF handle that raised the transfer complete event.
 * @note Runs in GPDMA1 channel 6 interrupt context.
 */
void HAL_MDF_AcqCpltCallback(MDF_HandleTypeDef *hmdf)
{
    drv_pdm_dma_chunk_callback_t callback = chunk_callback;

    if ((hmdf != NULL) && (hmdf->hdma == &dma) && (callback != NULL))
    {
        callback();
    }
}
