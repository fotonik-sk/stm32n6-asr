/**
 * @file     bsp_console.h
 * @brief    Board console: the UART wired to the ST-LINK virtual COM port.
 * @details  Initialization only; after it succeeds the data path is the
 *           drv_uart.h API (drv_uart_write(), drv_uart_write_direct(), ...).
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_BSP_BSP_CONSOLE_H
#define HARDWARE_BSP_BSP_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Route the console pins and initialize the console UART.
 * @retval 0 The console is ready or was already ready.
 * @retval -1 The UART driver rejected the configuration or HAL failed.
 */
int bsp_console_init(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_BSP_BSP_CONSOLE_H */
