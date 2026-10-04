/**
 * @file     drv_npu.c
 * @brief    STM32N6 NPU power, clock and bus-access bring-up; implements drv_npu.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "drv_npu.h"

#include "mcu_rif.h"
#include "stm32n6xx_hal.h"

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Clock the NPU SRAMs and take them out of shutdown.
 * @details AXISRAM3..6 are clock-gated and powered down after reset, and so is
 *          the RAM behind the NPU cache. AXISRAM1/2 are already running: the
 *          application and the FSBL live in them.
 */
static void npu_enable_memories(void)
{
    __HAL_RCC_AXISRAM3_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM4_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM5_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM6_MEM_CLK_ENABLE();
    __HAL_RCC_CACHEAXIRAM_MEM_CLK_ENABLE();

    __HAL_RCC_RAMCFG_CLK_ENABLE();
    CLEAR_BIT(RAMCFG_SRAM3_AXI->CR, RAMCFG_CR_SRAMSD);
    CLEAR_BIT(RAMCFG_SRAM4_AXI->CR, RAMCFG_CR_SRAMSD);
    CLEAR_BIT(RAMCFG_SRAM5_AXI->CR, RAMCFG_CR_SRAMSD);
    CLEAR_BIT(RAMCFG_SRAM6_AXI->CR, RAMCFG_CR_SRAMSD);
}

/**
 * @brief Keep everything an inference touches clocked while the CPU sleeps.
 * @details The inference thread blocks while the NPU runs; when no other thread
 *          is ready the CPU may enter WFI, and a gated clock would stall the NPU.
 */
static void npu_keep_clocks_in_sleep(void)
{
    __HAL_RCC_NPU_CLK_SLEEP_ENABLE();
    __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE();
    __HAL_RCC_AXISRAM1_MEM_CLK_SLEEP_ENABLE();
    __HAL_RCC_AXISRAM2_MEM_CLK_SLEEP_ENABLE();
    __HAL_RCC_AXISRAM3_MEM_CLK_SLEEP_ENABLE();
    __HAL_RCC_AXISRAM4_MEM_CLK_SLEEP_ENABLE();
    __HAL_RCC_AXISRAM5_MEM_CLK_SLEEP_ENABLE();
    __HAL_RCC_AXISRAM6_MEM_CLK_SLEEP_ENABLE();
    __HAL_RCC_CACHEAXIRAM_MEM_CLK_SLEEP_ENABLE();
    __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE();
    __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE();
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void drv_npu_init(void)
{
    npu_enable_memories();

    __HAL_RCC_NPU_CLK_ENABLE();
    __HAL_RCC_NPU_FORCE_RESET();
    __HAL_RCC_NPU_RELEASE_RESET();

    __HAL_RCC_CACHEAXI_CLK_ENABLE();
    __HAL_RCC_CACHEAXI_FORCE_RESET();
    __HAL_RCC_CACHEAXI_RELEASE_RESET();

    npu_keep_clocks_in_sleep();
    mcu_rif_grant_cpu_identity(RIF_MASTER_INDEX_NPU, RIF_RISC_PERIPH_INDEX_NPU);
    /* Last: a firewall is programmed only once the memory behind it is clocked. */
    mcu_rif_open_memory_firewalls();
}
