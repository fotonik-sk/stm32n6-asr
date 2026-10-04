/**
 * @file     drv_pdm.h
 * @brief    PDM microphone capture (MDF1 filter 0 + GPDMA1 channel 6) into a
 *           chunked ring buffer.
 * @details  Board-agnostic: the board routes the microphone clock to MDF1 CCK0
 *           and its data to SDI0, and supplies the pin setup and interrupt
 *           priority (bsp_mic.c). The filter chain turns a PDM stream clocked at
 *           the MDF output clock into 16 kHz PCM16.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_DRIVERS_PDM_DRV_PDM_H
#define HARDWARE_DRIVERS_PDM_DRV_PDM_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Result of a capture operation. */
typedef enum
{
    DRV_PDM_OK = 0,   /**< The operation completed. */
    DRV_PDM_ERROR,    /**< Invalid argument, hardware failure, or not initialized. */
    DRV_PDM_UNDERRUN, /**< Fewer complete chunks are stored than were requested. */
    DRV_PDM_OVERRUN   /**< The DMA reached the requested chunks while they were copied. */
} drv_pdm_status_t;

/** @brief Notification raised once per completed chunk, in interrupt context. */
typedef void (*drv_pdm_chunk_callback_t)(void *context);

/** @brief Capture geometry, completion notification and board hooks. */
typedef struct
{
    uint32_t chunk_samples; /**< PCM16 samples the DMA writes between two chunk interrupts. */
    uint32_t chunk_count;   /**< Chunks in the ring; at least 2. */
    drv_pdm_chunk_callback_t chunk_callback; /**< Optional chunk notification, may be NULL. */
    void *callback_context; /**< Argument passed back to @p chunk_callback. */
    void (*init_pins)(void); /**< Board hook that routes CCK0/SDI0; called once
                                  MDF1 is clocked, before the capture queue is
                                  linked. Filled in by bsp_mic_start(). */
    uint32_t irq_priority;  /**< GPDMA1 channel 6 preemption priority. Filled in
                                 by bsp_mic_start(). */
} drv_pdm_config_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Configure the microphone and its capture ring, then start acquisition.
 * @param[in] config Capture geometry, optional chunk notification and board hooks.
 * @retval DRV_PDM_OK Acquisition is running.
 * @retval DRV_PDM_ERROR The geometry does not fit the capture area, the pin hook
 *         is missing, or a hardware step failed.
 * @pre Call once from the startup thread; initialization is not safe to call
 *      concurrently.
 * @note The driver owns the capture memory, so no buffer is passed in and no
 *       cache maintenance is required of the caller. The geometry must satisfy
 *       @c chunk_samples * @c chunk_count <= the capture area configured in
 *       @c dma/drv_pdm_dma_config.h, which defaults to 1600 PCM16 samples
 *       (100 ms at 16 kHz). Acquisition is continuous: the DMA loops over
 *       @c chunk_count linked-list nodes and raises one interrupt per chunk.
 * @warning Acquisition can be started only once per reset. drv_pdm_stop() ends
 *          capture permanently; a later drv_pdm_init() returns DRV_PDM_ERROR because
 *          the HAL keeps the MDF instance registered.
 */
drv_pdm_status_t drv_pdm_init(const drv_pdm_config_t *config);

/**
 * @brief Stop acquisition and mask the capture interrupt.
 * @retval DRV_PDM_OK Acquisition stopped.
 * @retval DRV_PDM_ERROR The HAL stop sequence failed.
 * @note Stored chunks stay readable, but capture cannot be restarted; see the
 *       warning on drv_pdm_init().
 */
drv_pdm_status_t drv_pdm_stop(void);

/**
 * @brief Return the PCM16 sample count of one chunk.
 * @return Samples per chunk, or 0 before initialization.
 * @note Use it to size the destination passed to drv_pdm_read(): a request of
 *       @c n chunks needs @c n * drv_pdm_chunk_samples() samples.
 */
uint32_t drv_pdm_chunk_samples(void);

/**
 * @brief Return the byte count of one chunk.
 * @return Bytes per chunk, or 0 before initialization.
 */
uint32_t drv_pdm_chunk_bytes(void);

/**
 * @brief Return how many chunks the ring holds in total.
 * @return Configured chunk count, or 0 before initialization.
 * @note drv_pdm_read() accepts at most @c chunk_count - 1 chunks per call, because
 *       the DMA always owns the chunk it is currently filling.
 */
uint32_t drv_pdm_chunk_capacity(void);

/**
 * @brief Return the number of complete chunks waiting to be read.
 * @return Stored chunk count, or 0 before initialization.
 */
uint32_t drv_pdm_available_chunks(void);

/**
 * @brief Copy the oldest complete chunks out of the ring and release them.
 * @param[out] destination Array that receives the PCM16 samples.
 * @param[in] destination_samples Capacity of @p destination in PCM16 samples.
 * @param[in] chunks Number of chunks to copy, from 1 to drv_pdm_chunk_capacity() - 1.
 * @retval DRV_PDM_OK Every requested chunk was copied and released.
 * @retval DRV_PDM_ERROR Invalid argument, or @p destination is too small.
 * @retval DRV_PDM_UNDERRUN Fewer chunks are stored; nothing was copied.
 * @retval DRV_PDM_OVERRUN The DMA reached the copied chunks; the data is discarded
 *         and the read position is resynchronized to the newest audio.
 * @pre Call from a ThreadX thread with interrupts enabled; calls must be
 *      serialized with each other and with drv_pdm_skip().
 * @note The copy runs with interrupts enabled; only the position bookkeeping is
 *       done with interrupts masked. Copying rather than lending a pointer is
 *       what makes DRV_PDM_OVERRUN detectable before the caller uses the samples.
 */
drv_pdm_status_t drv_pdm_read(int16_t *destination, uint32_t destination_samples, uint32_t chunks);

/**
 * @brief Release the oldest complete chunks without copying them.
 * @param[in] chunks Number of chunks to drop.
 * @retval DRV_PDM_OK The chunks were released.
 * @retval DRV_PDM_ERROR Invalid argument, or the driver is not initialized.
 * @retval DRV_PDM_UNDERRUN Fewer chunks are stored; nothing was released.
 */
drv_pdm_status_t drv_pdm_skip(uint32_t chunks);

/**
 * @brief Read and clear the number of chunks lost because the reader was late.
 * @return Chunks the DMA overwrote before they were read since the previous
 *         call; 0 when the reader kept up.
 * @note Poll it next to drv_pdm_read() to detect a consumer that cannot sustain the
 *       capture rate. A non-zero value means audio is missing from the stream.
 */
uint32_t drv_pdm_take_dropped_chunks(void);

/**
 * @brief Dispatch the capture DMA interrupt.
 * @note Called by GPDMA1_Channel6_IRQHandler().
 */
void drv_pdm_irq_handler(void);

/* The reader side above (chunk geometry, available/read/skip/take_dropped) is
   implemented in ring/drv_pdm_ring.c, the interrupt dispatch in dma/drv_pdm_dma.c. */

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_DRIVERS_PDM_DRV_PDM_H */
