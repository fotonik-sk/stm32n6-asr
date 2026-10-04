/**
 * @file     stm32n6570dk.h
 * @brief    STM32N6570-DK (MB1939) board description.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_BSP_STM32N6570DK_H
#define HARDWARE_BSP_STM32N6570DK_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "stm32n6xx_hal.h"

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/* ST BSP configuration. */

#define BOARD_NAME          "STM32N6570-DK"

/* Optional ST BSP interfaces disabled for this project. */
#define USE_COM_LOG         0U
#define USE_BSP_COM_FEATURE 0U

/* Default priorities required by the ST BSP implementation. */
#define BSP_BUTTON_USER1_IT_PRIORITY 15U
#define BSP_BUTTON_TAMP_IT_PRIORITY  15U

/* Memory map: base addresses of the external memories only. */

/** @brief MX66UW1G45G NOR on XSPI2, mapped by the FSBL; the app executes from it. */
#define BOARD_NOR_BASE_ADDR          0x70000000UL
/** @brief Application slot in NOR (signing header first, vectors at +0x400). */
#define BOARD_NOR_APP_BASE_ADDR      0x70100000UL

/** @brief APS256XX PSRAM on XSPI1, mapped by bsp_memory_init(). */
#define BOARD_PSRAM_BASE_ADDR        0x90000000UL
#define BOARD_PSRAM_SIZE_BYTES       (32UL * 1024UL * 1024UL)
/**
 * @brief Highest XSPI1 kernel clock the PSRAM runs at.
 * @note XSPI1 stays on its reset-default HCLK source (errata ES0620), so this
 *       only bounds HCLK.
 */
#define BOARD_PSRAM_MAX_CLOCK_HZ     200000000UL

/* Console: USART1 -> ST-LINK VCP. */

#define BOARD_CONSOLE_UART           USART1
/** @note USART1 runs from PCLK2 (200 MHz): BRR 217, 0.006 % off. */
#define BOARD_CONSOLE_BAUD_RATE      921600U
#define BOARD_CONSOLE_IRQ_PRIORITY   5U
#define BOARD_CONSOLE_GPIO_PORT      GPIOE
#define BOARD_CONSOLE_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define BOARD_CONSOLE_TX_PIN         GPIO_PIN_5
#define BOARD_CONSOLE_RX_PIN         GPIO_PIN_6
#define BOARD_CONSOLE_GPIO_AF        GPIO_AF7_USART1

/* Microphone: MP34DT06J PDM -> MDF1 CCK0/SDI0. */

#define BOARD_MIC_IRQ_PRIORITY       5U
#define BOARD_MIC_GPIO_PORT          GPIOE
#define BOARD_MIC_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOE_CLK_ENABLE()
#define BOARD_MIC_CLOCK_PIN          GPIO_PIN_8 /**< MDF1_CCK0 */
#define BOARD_MIC_DATA_PIN           GPIO_PIN_2 /**< MDF1_SDI0 */
#define BOARD_MIC_GPIO_AF            GPIO_AF4_MDF1

/* LCD: 800x480 RGB565 parallel panel on LTDC. The timings below are the
   cumulative LTDC register values (see drv_ltdc_config_t): sync, + back porch,
   + active area, + front porch, each counted in pixels or lines and minus one. */
#define BOARD_LCD_WIDTH                        800U
#define BOARD_LCD_HEIGHT                       480U
#define BOARD_LCD_PIXEL_CLOCK_HZ               20000000UL
#define BOARD_LCD_HORIZONTAL_SYNC              4U
#define BOARD_LCD_VERTICAL_SYNC                4U
#define BOARD_LCD_ACCUMULATED_HBP              12U
#define BOARD_LCD_ACCUMULATED_VBP              12U
#define BOARD_LCD_ACCUMULATED_ACTIVE_WIDTH     812U
#define BOARD_LCD_ACCUMULATED_ACTIVE_HEIGHT    492U
#define BOARD_LCD_TOTAL_WIDTH                  820U
#define BOARD_LCD_TOTAL_HEIGHT                 500U

/** @brief LTDC RGB bus (AF14) pins per port. */
#define BOARD_LCD_GPIO_AF            GPIO_AF14_LCD
#define BOARD_LCD_GPIOA_PINS         (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7 | \
                                      GPIO_PIN_8 | GPIO_PIN_15)
#define BOARD_LCD_GPIOB_PINS         (GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_11 | GPIO_PIN_12 | \
                                      GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)
#define BOARD_LCD_GPIOD_PINS         (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_15)
#define BOARD_LCD_GPIOE_PINS         (GPIO_PIN_11)
#define BOARD_LCD_GPIOG_PINS         (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_8 | \
                                      GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15)
#define BOARD_LCD_GPIOH_PINS         (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6)

/** @brief Panel control outputs, held low until the LTDC scans a valid frame. */
#define BOARD_LCD_NRST_PORT          GPIOE
#define BOARD_LCD_NRST_PIN           GPIO_PIN_1 /**< LCD_NRST, active low */
#define BOARD_LCD_CTRL_PORT          GPIOQ
#define BOARD_LCD_CTRL_PINS          (GPIO_PIN_3 | GPIO_PIN_6) /**< LCD_ONOFF, LCD_BL_CTRL */

#endif /* HARDWARE_BSP_STM32N6570DK_H */
