/**
 * @file     drv_uart.c
 * @brief    Unbuffered interrupt-driven U(S)ART driver implementation.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/
#include "drv_uart.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief Maximum number of polls used by the emergency output path. */
#define DRV_UART_DIRECT_POLL_LIMIT 1000000U

/** @brief Maximum emergency text length, excluding the terminating NUL. */
#define DRV_UART_DIRECT_TEXT_LIMIT 511U

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief HAL handle owned by the driver. */
static UART_HandleTypeDef uart;

/** @brief Indicates that the instance and its interrupt are initialised. */
static volatile bool ready;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Wait for a UART flag without relying on HAL ticks or interrupts.
 * @param[in] flag HAL UART flag to poll.
 * @return true when the flag is set; false after the bounded polling limit.
 */
static bool direct_wait_flag(uint32_t flag)
{
    for (uint32_t attempt = 0U; attempt < DRV_UART_DIRECT_POLL_LIMIT; attempt++)
    {
        if (__HAL_UART_GET_FLAG(&uart, flag) != RESET)
        {
            return true;
        }
    }
    return false;
}


/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

int drv_uart_init(const drv_uart_config_t *config)
{
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    if (ready)
    {
        __set_PRIMASK(primask);
        return 0;
    }
    if ((config == NULL) || (config->instance != USART1) || (config->init_pins == NULL))
    {
        __set_PRIMASK(primask);
        return -1;
    }

    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();
    config->init_pins();

    uart.Instance = config->instance;
    uart.Init.BaudRate = config->baud_rate;
    uart.Init.WordLength = UART_WORDLENGTH_8B;
    uart.Init.StopBits = UART_STOPBITS_1;
    uart.Init.Parity = UART_PARITY_NONE;
    uart.Init.Mode = UART_MODE_TX_RX;
    uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart.Init.OverSampling = UART_OVERSAMPLING_16;
    uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_OK != HAL_UART_Init(&uart))
    {
        __set_PRIMASK(primask);
        return -1;
    }

    uart.Instance->CR1 &= ~USART_CR1_FIFOEN;
    HAL_NVIC_SetPriority(USART1_IRQn, config->irq_priority, 0U);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    ready = true;

    __set_PRIMASK(primask);
    return 0;
}

bool drv_uart_write(const uint8_t *data, size_t length)
{
    if (!ready || (data == NULL) || (length == 0U) || (length > UINT16_MAX))
    {
        return false;
    }

    return HAL_UART_Transmit_IT(&uart, data, (uint16_t)length) == HAL_OK;
}

bool drv_uart_tx_ready(void)
{
    HAL_UART_StateTypeDef state;

    if (!ready)
    {
        return false;
    }

    state = HAL_UART_GetState(&uart);
    return (state == HAL_UART_STATE_READY) || (state == HAL_UART_STATE_BUSY_RX);
}

bool drv_uart_read(uint8_t *data, size_t length)
{
    if (!ready || (data == NULL) || (length == 0U) || (length > UINT16_MAX))
    {
        return false;
    }

    return HAL_UART_Receive_IT(&uart, data, (uint16_t)length) == HAL_OK;
}

bool drv_uart_rx_ready(void)
{
    HAL_UART_StateTypeDef state;

    if (!ready)
    {
        return false;
    }

    state = HAL_UART_GetState(&uart);
    return (state == HAL_UART_STATE_READY) || (state == HAL_UART_STATE_BUSY_TX);
}

bool drv_uart_write_direct(const char *text)
{
    size_t length = 0U;

    if ((text == NULL) || !ready || (__get_PRIMASK() == 0U))
    {
        return false;
    }

    while ((length < DRV_UART_DIRECT_TEXT_LIMIT) && (text[length] != '\0'))
    {
        if (!direct_wait_flag(UART_FLAG_TXE))
        {
            return false;
        }
        uart.Instance->TDR = (uint8_t)text[length++];
    }

    /* Wait for the final stop bit before the caller halts the processor. */
    if (!direct_wait_flag(UART_FLAG_TC))
    {
        return false;
    }
    return text[length] == '\0';
}

void drv_uart_irq_handler(void)
{
    HAL_UART_IRQHandler(&uart);
}
