/**
 * @file     mcu_core.c
 * @brief    Cortex-M55 core setup; implements mcu_core.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mcu_core.h"

#include "stm32n6xx_hal.h"

/*******************************************************************************/
/*                              EXTERNAL VARIABLES                             */
/*******************************************************************************/

/** @brief First byte of the NONCACHEABLE region (STM32N657XX_XIP.ld). */
extern uint8_t __noncacheable_start[];
/** @brief First byte past the NONCACHEABLE region (STM32N657XX_XIP.ld). */
extern uint8_t __noncacheable_end[];

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Configure the MPU: mark the linker's NONCACHEABLE region as non-cacheable.
 * @note The region has a fixed address and size in the linker script. The MPU
 *       limit is the last byte inside the region, hence the "- 1".
 */
static void configure_mpu(void)
{
    MPU_Region_InitTypeDef region = {0};
    MPU_Attributes_InitTypeDef attributes = {0};
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    HAL_MPU_Disable();

    region.Enable = MPU_REGION_ENABLE;
    region.Number = MPU_REGION_NUMBER0;
    region.BaseAddress = (uint32_t)__noncacheable_start;
    region.LimitAddress = (uint32_t)__noncacheable_end - 1U;
    region.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
    region.AccessPermission = MPU_REGION_ALL_RW;
    region.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    region.DisablePrivExec = MPU_PRIV_INSTRUCTION_ACCESS_DISABLE;
    region.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    HAL_MPU_ConfigRegion(&region);

    attributes.Number = MPU_ATTRIBUTES_NUMBER0;
    attributes.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
    HAL_MPU_ConfigMemoryAttributes(&attributes);

    HAL_MPU_Enable(MPU_HFNMI_PRIVDEF);

    __set_PRIMASK(primask);
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void mcu_core_init(void)
{
    /* MPU first: the caches must never see the NONCACHEABLE region as cacheable. */
    configure_mpu();
    SCB_EnableICache();
    SCB_EnableDCache();
}
