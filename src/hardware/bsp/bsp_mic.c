/**
 * @file     bsp_mic.c
 * @brief    Board microphone wiring; implements bsp_mic.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "bsp_mic.h"

#include <stddef.h>

#include "boards.h"

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Route the microphone clock and data lines to MDF1, secure and unprivileged.
 * @note Called by the PDM driver once MDF1 is clocked.
 */
static void init_pins(void)
{
    GPIO_InitTypeDef gpio = {0};

    BOARD_MIC_GPIO_CLK_ENABLE();

    gpio.Pin = BOARD_MIC_CLOCK_PIN | BOARD_MIC_DATA_PIN;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = BOARD_MIC_GPIO_AF;
    HAL_GPIO_Init(BOARD_MIC_GPIO_PORT, &gpio);

    HAL_GPIO_ConfigPinAttributes(BOARD_MIC_GPIO_PORT, BOARD_MIC_DATA_PIN, GPIO_PIN_SEC | GPIO_PIN_NPRIV);
    HAL_GPIO_ConfigPinAttributes(BOARD_MIC_GPIO_PORT, BOARD_MIC_CLOCK_PIN, GPIO_PIN_SEC | GPIO_PIN_NPRIV);
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

drv_pdm_status_t bsp_mic_start(const drv_pdm_config_t *capture)
{
    drv_pdm_config_t config;

    if (capture == NULL)
    {
        return DRV_PDM_ERROR;
    }

    config = *capture;
    config.init_pins = init_pins;
    config.irq_priority = BOARD_MIC_IRQ_PRIORITY;
    return drv_pdm_init(&config);
}
