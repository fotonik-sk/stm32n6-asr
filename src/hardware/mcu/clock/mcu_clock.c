/**
 * @file     mcu_clock.c
 * @brief    STM32N6 PLL configuration; implements mcu_clock.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mcu_clock.h"

#include "stm32n6xx_hal.h"

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

bool mcu_clock_init_system_pll(void)
{
    RCC_OscInitTypeDef hsi_config = {0};
    RCC_OscInitTypeDef pll_config = {0};
    RCC_ClkInitTypeDef clock_config = {0};

    if (HAL_OK != HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY))
    {
        return false;
    }

    if (HAL_OK != HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1))
    {
        return false;
    }

    hsi_config.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    hsi_config.HSIState = RCC_HSI_ON;
    hsi_config.HSIDiv = RCC_HSI_DIV1;
    hsi_config.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    hsi_config.PLL1.PLLState = RCC_PLL_NONE;
    hsi_config.PLL2.PLLState = RCC_PLL_NONE;
    hsi_config.PLL3.PLLState = RCC_PLL_NONE;
    hsi_config.PLL4.PLLState = RCC_PLL_NONE;
    if (HAL_OK != HAL_RCC_OscConfig(&hsi_config))
    {
        return false;
    }

    HAL_RCC_GetClockConfig(&clock_config);
    if ((RCC_CPUCLKSOURCE_IC1 == clock_config.CPUCLKSource) ||
        (RCC_SYSCLKSOURCE_IC2_IC6_IC11 == clock_config.SYSCLKSource))
    {
        clock_config.ClockType = RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK;
        clock_config.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
        clock_config.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
        if (HAL_OK != HAL_RCC_ClockConfig(&clock_config))
        {
            return false;
        }
    }

    /* PLL1 = 64 MHz HSI / M 4 * N 150 / (P1 1 * P2 1) = 2400 MHz. */
    pll_config.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    pll_config.PLL1.PLLState = RCC_PLL_ON;
    pll_config.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
    pll_config.PLL1.PLLM = 4U;
    pll_config.PLL1.PLLN = 150U;
    pll_config.PLL1.PLLFractional = 0U;
    pll_config.PLL1.PLLP1 = 1U;
    pll_config.PLL1.PLLP2 = 1U;
    pll_config.PLL2.PLLState = RCC_PLL_NONE;
    pll_config.PLL3.PLLState = RCC_PLL_NONE;
    pll_config.PLL4.PLLState = RCC_PLL_NONE;
    if (HAL_OK != HAL_RCC_OscConfig(&pll_config))
    {
        return false;
    }

    /* From PLL1: IC1 /4 = 600 MHz CPU, IC2 /6 = 400 MHz AXI, IC6 /3 = 800 MHz
       NPU, IC11 /6 = 400 MHz AXISRAM3..6; HCLK = AXI /2 = 200 MHz. */
    clock_config.ClockType = RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                             RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK4 |
                             RCC_CLOCKTYPE_PCLK5;
    clock_config.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
    clock_config.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
    clock_config.AHBCLKDivider = RCC_HCLK_DIV2;
    clock_config.APB1CLKDivider = RCC_APB1_DIV1;
    clock_config.APB2CLKDivider = RCC_APB2_DIV1;
    clock_config.APB4CLKDivider = RCC_APB4_DIV1;
    clock_config.APB5CLKDivider = RCC_APB5_DIV1;
    clock_config.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    clock_config.IC1Selection.ClockDivider = 4U;
    clock_config.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    clock_config.IC2Selection.ClockDivider = 6U;
    clock_config.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    clock_config.IC6Selection.ClockDivider = 3U;
    clock_config.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    clock_config.IC11Selection.ClockDivider = 6U;
    if (HAL_OK != HAL_RCC_ClockConfig(&clock_config))
    {
        return false;
    }

    return true;
}

bool mcu_clock_enable_audio_pll(void)
{
    RCC_OscInitTypeDef oscillator_config = {0};

    /* PLL3 = 64 MHz HSI / M 8 * N 172 / (P1 7 * P2 4) = 49.14 MHz; drv_pdm_mdf
       divides it down to the microphone clock. */
    oscillator_config.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    oscillator_config.PLL1.PLLState = RCC_PLL_NONE;
    oscillator_config.PLL2.PLLState = RCC_PLL_NONE;
    oscillator_config.PLL3.PLLState = RCC_PLL_ON;
    oscillator_config.PLL3.PLLSource = RCC_PLLSOURCE_HSI;
    oscillator_config.PLL3.PLLFractional = 0U;
    oscillator_config.PLL3.PLLM = 8U;
    oscillator_config.PLL3.PLLN = 172U;
    oscillator_config.PLL3.PLLP1 = 7U;
    oscillator_config.PLL3.PLLP2 = 4U;
    oscillator_config.PLL4.PLLState = RCC_PLL_NONE;
    return HAL_OK == HAL_RCC_OscConfig(&oscillator_config);
}
