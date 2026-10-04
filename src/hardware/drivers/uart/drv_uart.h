/**
 * @file     drv_uart.h
 * @brief    Unbuffered interrupt-driven U(S)ART driver.
 * @details  Board-agnostic: the instance, line rate, interrupt priority and pin
 *           setup come from the BSP (bsp_console.c). One instance per image.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_DRIVERS_UART_DRV_UART_H
#define HARDWARE_DRIVERS_UART_DRV_UART_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "stm32n6xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Instance, line settings and board hook for drv_uart_init(). */
typedef struct
{
    USART_TypeDef *instance; /**< Peripheral; USART1 is the only one supported so far. */
    uint32_t baud_rate;      /**< Line rate, 8N1. */
    uint32_t irq_priority;   /**< NVIC preemption priority of the instance interrupt. */
    void (*init_pins)(void); /**< Board hook that routes TX/RX; called once the
                                  peripheral is clocked, before it is configured. */
} drv_uart_config_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Clock, reset and configure the instance, then enable its interrupt.
 * @param[in] config Instance, line settings and pin hook.
 * @retval 0 The UART is initialised or was already initialised.
 * @retval -1 Unsupported instance, missing hook or HAL initialisation failure.
 */
int drv_uart_init(const drv_uart_config_t *config);

/**
 * @brief Start an interrupt-driven transfer of the complete array.
 * @param[in] data Array to transmit.
 * @param[in] length Number of bytes to transmit, from 1 to UINT16_MAX.
 * @return true if HAL started the whole transfer; false otherwise.
 * @note The driver does not copy data. Keep data valid and unchanged until
 * drv_uart_tx_ready() returns true.
 */
bool drv_uart_write(const uint8_t *data, size_t length);

/**
 * @brief Check whether an interrupt-driven transmit operation is complete.
 * @return true when the transmitter is ready; false before initialisation or
 * while a transfer is in progress.
 */
bool drv_uart_tx_ready(void);

/**
 * @brief Start interrupt-driven reception into a caller-owned array.
 * @param[out] data Array that receives the incoming bytes.
 * @param[in] length Number of bytes to receive, from 1 to UINT16_MAX.
 * @return true if HAL started reception of the whole array; false otherwise.
 * @note Keep data valid and do not access it until drv_uart_rx_ready() returns
 * true.
 */
bool drv_uart_read(uint8_t *data, size_t length);

/**
 * @brief Check whether an interrupt-driven receive operation is complete.
 * @return true when the receiver is ready; false before initialisation or
 * while a reception is in progress.
 */
bool drv_uart_rx_ready(void);

/**
 * @brief Transmit a NUL-terminated string directly without RTOS services.
 * @param[in] text Text containing at most 511 bytes before the terminating NUL.
 * @return true after physical transmission of the entire string; false on error.
 * @pre UART is initialised and interrupts are masked through PRIMASK.
 * @note Intended only for the fatal-error path. It uses bounded register
 * polling because an interrupt-driven HAL transfer cannot complete here.
 */
bool drv_uart_write_direct(const char *text);

/**
 * @brief Dispatch the instance interrupt to HAL.
 * @note Called by the instance IRQ handler in stm32n6xx_it.c.
 */
void drv_uart_irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_DRIVERS_UART_DRV_UART_H */
