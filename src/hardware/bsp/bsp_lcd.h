/**
 * @file     bsp_lcd.h
 * @brief    Board display: the 800x480 RGB565 panel on the LTDC.
 * @details  Initialization only; after it succeeds the frame path is the
 *           drv_ltdc.h API (drv_ltdc_get_draw_buffer(), drv_ltdc_present(), ...).
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_BSP_BSP_LCD_H
#define HARDWARE_BSP_BSP_LCD_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "stm32n6xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Bring up the LTDC with the panel timings, then switch the panel on.
 * @return HAL_OK on success, or the hardware configuration error status.
 */
HAL_StatusTypeDef bsp_lcd_init(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_BSP_BSP_LCD_H */
