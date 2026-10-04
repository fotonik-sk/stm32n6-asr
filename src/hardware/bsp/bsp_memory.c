/**
 * @file     bsp_memory.c
 * @brief    Board external memories; implements bsp_memory.h over the ST BSP
 *           XSPI driver and the APS256XX component.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "bsp_memory.h"

#include "boards.h"
#include "stm32n6570_discovery_errno.h"
#include "stm32n6570_discovery_xspi.h"

/*******************************************************************************/
/*                              EXTERNAL VARIABLES                             */
/*******************************************************************************/

/** @brief ST BSP PSRAM context; tells whether the PSRAM is already mapped. */
extern XSPI_RAM_Ctx_t XSPI_Ram_Ctx[XSPI_RAM_INSTANCES_NUMBER];

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

HAL_StatusTypeDef bsp_memory_init(void)
{
    RCC_PeriphCLKInitTypeDef clock_config = {0};

    /* NOR: inherited from the FSBL, never re-established (see bsp_memory.h).
       PSRAM: the APS256XX on XSPI1, memory-mapped unless it already is. */
    if (XSPI_Ram_Ctx[0U].IsInitialized == XSPI_ACCESS_MMP)
    {
        return HAL_OK;
    }

    /* Keep XSPI1 on its reset-default HCLK source: STM32N6 errata ES0620
       documents a BootROM failure after system reset when XSPI1SEL differs
       from its reset value. HCLK already runs at 200 MHz, so the PSRAM clock
       must be adjusted only with the XSPI peripheral prescaler. */
    if (HAL_RCC_GetHCLKFreq() > BOARD_PSRAM_MAX_CLOCK_HZ)
    {
        return HAL_ERROR;
    }
    clock_config.PeriphClockSelection = RCC_PERIPHCLK_XSPI1;
    clock_config.Xspi1ClockSelection = RCC_XSPI1CLKSOURCE_HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&clock_config) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (BSP_XSPI_RAM_Init(0U) != BSP_ERROR_NONE)
    {
        return HAL_ERROR;
    }
    if (BSP_XSPI_RAM_EnableMemoryMappedMode(0U) != BSP_ERROR_NONE)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}
