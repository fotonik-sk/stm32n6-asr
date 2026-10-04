/**
 * @file     bsp_lcd.c
 * @brief    Board display wiring and panel timings; implements bsp_lcd.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "bsp_lcd.h"

#include "boards.h"
#include "drv_ltdc.h"

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/**
 * @brief The two RGB565 framebuffers the LTDC scans.
 * @details 750 KB each, so they live in external PSRAM (.lcd_framebuffer of the
 *          linker script, NOLOAD); cache-line aligned for the D-cache clean
 *          drv_ltdc does after each frame. drv_ltdc_init() clears both before
 *          the panel is switched on, so the startup need not zero them.
 */
static uint16_t framebuffer[2][BOARD_LCD_WIDTH * BOARD_LCD_HEIGHT]
    __attribute__((section(".lcd_framebuffer"), aligned(32)));

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Configure the LTDC RGB signals and the panel control outputs.
 * @details The control outputs remain low while the LTDC and its framebuffers
 *          are initialized, preventing invalid pixels from reaching the display.
 * @note Called by drv_ltdc_init() once the pixel clock is set.
 */
static void init_pins(void)
{
    GPIO_InitTypeDef gpio = {0};

    HAL_PWREx_EnableVddIO4();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOQ_CLK_ENABLE();

    HAL_GPIO_WritePin(BOARD_LCD_NRST_PORT, BOARD_LCD_NRST_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BOARD_LCD_CTRL_PORT, BOARD_LCD_CTRL_PINS, GPIO_PIN_RESET);
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Pin = BOARD_LCD_NRST_PIN;
    HAL_GPIO_Init(BOARD_LCD_NRST_PORT, &gpio);
    gpio.Pin = BOARD_LCD_CTRL_PINS;
    HAL_GPIO_Init(BOARD_LCD_CTRL_PORT, &gpio);

    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Alternate = BOARD_LCD_GPIO_AF;
    gpio.Pin = BOARD_LCD_GPIOA_PINS;
    HAL_GPIO_Init(GPIOA, &gpio);
    gpio.Pin = BOARD_LCD_GPIOB_PINS;
    HAL_GPIO_Init(GPIOB, &gpio);
    gpio.Pin = BOARD_LCD_GPIOD_PINS;
    HAL_GPIO_Init(GPIOD, &gpio);
    gpio.Pin = BOARD_LCD_GPIOE_PINS;
    HAL_GPIO_Init(GPIOE, &gpio);
    gpio.Pin = BOARD_LCD_GPIOG_PINS;
    HAL_GPIO_Init(GPIOG, &gpio);
    gpio.Pin = BOARD_LCD_GPIOH_PINS;
    HAL_GPIO_Init(GPIOH, &gpio);
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

HAL_StatusTypeDef bsp_lcd_init(void)
{
    const drv_ltdc_config_t config = {
        .width = BOARD_LCD_WIDTH,
        .height = BOARD_LCD_HEIGHT,
        .pixel_clock_hz = BOARD_LCD_PIXEL_CLOCK_HZ,
        .horizontal_sync = BOARD_LCD_HORIZONTAL_SYNC,
        .vertical_sync = BOARD_LCD_VERTICAL_SYNC,
        .accumulated_hbp = BOARD_LCD_ACCUMULATED_HBP,
        .accumulated_vbp = BOARD_LCD_ACCUMULATED_VBP,
        .accumulated_active_width = BOARD_LCD_ACCUMULATED_ACTIVE_WIDTH,
        .accumulated_active_height = BOARD_LCD_ACCUMULATED_ACTIVE_HEIGHT,
        .total_width = BOARD_LCD_TOTAL_WIDTH,
        .total_height = BOARD_LCD_TOTAL_HEIGHT,
        .framebuffer0 = (uintptr_t)framebuffer[0],
        .framebuffer1 = (uintptr_t)framebuffer[1],
        .init_pins = init_pins,
    };
    HAL_StatusTypeDef status = drv_ltdc_init(&config);

    if (status != HAL_OK)
    {
        return status;
    }

    /* The first frame is valid: release the panel reset, switch it on. */
    HAL_GPIO_WritePin(BOARD_LCD_NRST_PORT, BOARD_LCD_NRST_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BOARD_LCD_CTRL_PORT, BOARD_LCD_CTRL_PINS, GPIO_PIN_SET);
    return HAL_OK;
}
