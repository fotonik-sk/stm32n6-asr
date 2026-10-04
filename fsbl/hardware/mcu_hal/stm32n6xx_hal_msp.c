/**
  ******************************************************************************
  * @file         stm32n6xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * Modifications Copyright (c) 2026 Nikita Prismakov.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*******************************************************************************/
/*                                  INCLUDES                                   */
/*******************************************************************************/

#include "main.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/**
 * @brief XSPIM port 2 pins on GPION (AF9): CLK PN6, NCS1 PN1, DQS0 PN0,
 *        IO0..IO7 PN2, PN3, PN4, PN5, PN8, PN9, PN10, PN11.
 */
#define NOR_XSPI_PINS (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | \
                       GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
                       GPIO_PIN_11)

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/** @brief Global MSP: power the VDDIO3 supply of the XSPI2 pins. */
void HAL_MspInit(void)
{
  HAL_PWREx_EnableVddIO3();
}

/**
 * @brief Clock XSPI2 and route its pins; called by HAL_XSPI_Init().
 * @param[in] hxspi XSPI handle being initialized.
 */
void HAL_XSPI_MspInit(XSPI_HandleTypeDef *hxspi)
{
  GPIO_InitTypeDef gpio = {0};
  RCC_PeriphCLKInitTypeDef clock_config = {0};

  if (hxspi->Instance != XSPI2)
  {
    return;
  }

  /* VDDIO3 powers the NOR pins at 1.8 V. */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddIO3();
  HAL_PWREx_ConfigVddIORange(PWR_VDDIO3, PWR_VDDIO_RANGE_1V8);

  /* XSPI2 from the safe 50 MHz source, IC3 = PLL1 2400 MHz / 48. The board's
     OTP has VDDIO3/VDDIO2 HSLV burned, but the 200 MHz HSLV path is skipped on
     purpose: 50 MHz works for any VDDIO/HSLV state, and slower flash is
     acceptable. No BSEC/OTP read is needed at a fixed speed. */
  clock_config.PeriphClockSelection = RCC_PERIPHCLK_XSPI2;
  clock_config.Xspi2ClockSelection = RCC_XSPI2CLKSOURCE_IC3;
  clock_config.ICSelection[RCC_IC3].ClockSelection = RCC_ICCLKSOURCE_PLL1;
  clock_config.ICSelection[RCC_IC3].ClockDivider = 48;
  if (HAL_RCCEx_PeriphCLKConfig(&clock_config) != HAL_OK)
  {
    error_handler();
  }

  __HAL_RCC_XSPIM_CLK_ENABLE();
  __HAL_RCC_XSPI2_CLK_ENABLE();

  __HAL_RCC_GPION_CLK_ENABLE();
  gpio.Pin = NOR_XSPI_PINS;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF9_XSPIM_P2;
  HAL_GPIO_Init(GPION, &gpio);
}

/**
 * @brief Release the XSPI2 clocks and pins; called by HAL_XSPI_DeInit().
 * @param[in] hxspi XSPI handle being de-initialized.
 */
void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef *hxspi)
{
  if (hxspi->Instance != XSPI2)
  {
    return;
  }

  __HAL_RCC_XSPIM_CLK_DISABLE();
  __HAL_RCC_XSPI2_CLK_DISABLE();
  HAL_GPIO_DeInit(GPION, NOR_XSPI_PINS);
}
