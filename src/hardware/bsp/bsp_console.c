/**
 * @file     bsp_console.c
 * @brief    Board console wiring; implements bsp_console.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "bsp_console.h"

#include "boards.h"
#include "drv_uart.h"

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Route TX and RX to the console UART; RX idles high through a pull-up.
 * @note Called by drv_uart_init() once the UART is clocked.
 */
static void init_pins(void)
{
    GPIO_InitTypeDef gpio = {0};

    BOARD_CONSOLE_GPIO_CLK_ENABLE();

    gpio.Pin = BOARD_CONSOLE_TX_PIN;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = BOARD_CONSOLE_GPIO_AF;
    HAL_GPIO_Init(BOARD_CONSOLE_GPIO_PORT, &gpio);

    gpio.Pin = BOARD_CONSOLE_RX_PIN;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BOARD_CONSOLE_GPIO_PORT, &gpio);
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

int bsp_console_init(void)
{
    const drv_uart_config_t config = {
        .instance = BOARD_CONSOLE_UART,
        .baud_rate = BOARD_CONSOLE_BAUD_RATE,
        .irq_priority = BOARD_CONSOLE_IRQ_PRIORITY,
        .init_pins = init_pins,
    };

    return drv_uart_init(&config);
}
