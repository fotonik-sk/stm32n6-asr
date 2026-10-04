/**
 * @file     drv_ltdc.h
 * @brief    LTDC driver: one RGB565 layer over two double-buffered framebuffers.
 * @details  Board-agnostic: panel geometry, timings, pixel clock, framebuffer
 *           addresses and pin setup come from the BSP (bsp_lcd.c). Sync and
 *           data-enable signals are active low and the pixel clock is inverted,
 *           as on every parallel-RGB panel used so far; add fields here before
 *           driving a panel that differs.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_DRIVERS_LTDC_DRV_LTDC_H
#define HARDWARE_DRIVERS_LTDC_DRV_LTDC_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "stm32n6xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Panel geometry, timings, framebuffers and board hook for drv_ltdc_init(). */
typedef struct
{
    uint16_t width;                  /**< Visible width in pixels; also the row stride. */
    uint16_t height;                 /**< Visible height in pixels. */
    uint32_t pixel_clock_hz;         /**< Must divide the PLL1 frequency exactly. */
    uint32_t horizontal_sync;        /**< LTDC HorizontalSync (HSW - 1). */
    uint32_t vertical_sync;          /**< LTDC VerticalSync (VSH - 1). */
    uint32_t accumulated_hbp;        /**< LTDC AccumulatedHBP. */
    uint32_t accumulated_vbp;        /**< LTDC AccumulatedVBP. */
    uint32_t accumulated_active_width;  /**< LTDC AccumulatedActiveW. */
    uint32_t accumulated_active_height; /**< LTDC AccumulatedActiveH. */
    uint32_t total_width;            /**< LTDC TotalWidth. */
    uint32_t total_height;           /**< LTDC TotalHeigh. */
    uintptr_t framebuffer0;          /**< First RGB565 buffer; cache-line aligned. */
    uintptr_t framebuffer1;          /**< Second RGB565 buffer; cache-line aligned. */
    void (*init_pins)(void);         /**< Board hook that routes the RGB bus; called
                                          after the pixel clock is set, before the
                                          LTDC is enabled. */
} drv_ltdc_config_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Initialize the LTDC layer and clear both framebuffers to white.
 * @param[in] config Panel description; copied, so it may live on the stack.
 * @details Layer 0 scans framebuffer 0. Later CPU writes require D-cache
 *          cleaning, which drv_ltdc_present() does. Initialization is not safe
 *          to call concurrently. A successful repeat call leaves the display
 *          unchanged.
 * @pre PLL1 runs and the memory holding the framebuffers is mapped.
 * @return HAL_OK on success, or the hardware configuration error status.
 */
HAL_StatusTypeDef drv_ltdc_init(const drv_ltdc_config_t *config);

/**
 * @brief Return the framebuffer available for CPU rendering.
 * @return Address of the hidden RGB565 buffer, or NULL before initialization.
 * @note The returned pointer changes after every successful drv_ltdc_present().
 */
uint16_t *drv_ltdc_get_draw_buffer(void);

/**
 * @brief Return the visible width of the display, which is also the row stride.
 * @return Width in pixels, or 0 before initialization.
 */
uint16_t drv_ltdc_get_width(void);

/**
 * @brief Return the visible height of the display.
 * @return Height in pixels, or 0 before initialization.
 */
uint16_t drv_ltdc_get_height(void);

/**
 * @brief Display the internally tracked drawing buffer and return the next one.
 * @return Previous front buffer after the vertical-blanking switch completes,
 *         or NULL when the LTDC is not ready or an LTDC operation fails.
 * @pre drv_ltdc_init() returned HAL_OK and the caller no longer modifies the frame.
 * @note Calls must be serialized and made with the HAL tick running.
 */
uint16_t *drv_ltdc_present(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_DRIVERS_LTDC_DRV_LTDC_H */
