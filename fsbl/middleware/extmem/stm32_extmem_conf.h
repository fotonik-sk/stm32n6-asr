/**
  ******************************************************************************
  * @file    stm32_extmem_conf.h
  * @author  GPM Application Team
  * @brief   Header configuration for extmem module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * Modifications Copyright (c) 2026 Nikita Prismakov.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef EXTMEM_CONF_H_
#define EXTMEM_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               ENABLED LAYERS                                */
/*******************************************************************************/

/* Read by the manager's headers below, so they come first. */

/** @brief SFDP NOR driver: on, for the MX66UW1G45G. */
#define EXTMEM_DRIVER_NOR_SFDP 1

/** @brief PSRAM driver: off, the application maps the PSRAM itself. */
#define EXTMEM_DRIVER_PSRAM    0

/** @brief XSPI serial abstraction layer: on. */
#define EXTMEM_SAL_XSPI        1

/*******************************************************************************/
/*                                  INCLUDES                                   */
/*******************************************************************************/

#include "stm32n6xx_hal.h"
#include "stm32_extmem.h"
#include "stm32_extmem_type.h"
#include "boot/stm32_boot_xip.h"

/*******************************************************************************/
/*                                  MEMORIES                                   */
/*******************************************************************************/

/** @brief HAL handle of XSPI2, initialized in fsbl/application/main.c. */
extern XSPI_HandleTypeDef nor_xspi;

/** @brief Index of each memory in extmem_list_config. */
enum
{
  EXTMEMORY_1 = 0, /**< MX66UW1G45G NOR on XSPI2. */
};

/*
 * Execute-in-place (XIP) boot: the FSBL leaves the NOR memory-mapped and jumps
 * to the application's vector table in place. The application is linked for
 * NOR (STM32N657XX_XIP.ld, 0x70100400).
 */

/** @brief Memory the application runs from: the NOR, mapped at 0x70000000. */
#define EXTMEM_MEMORY_BOOTXIP   EXTMEMORY_1

/** @brief Application partition in the NOR: 0x70100000. */
#define EXTMEM_XIP_IMAGE_OFFSET 0x00100000U

/** @brief Signing header in front of the vector table: 0x70100400. */
#define EXTMEM_HEADER_OFFSET    0x00000400U

/** @brief Memories the manager drives; defined in stm32_extmem.c (EXTMEM_C). */
extern EXTMEM_DefinitionTypeDef extmem_list_config[1];
#if defined(EXTMEM_C)
EXTMEM_DefinitionTypeDef extmem_list_config[1] =
{
  {
    .MemType = EXTMEM_NOR_SFDP,
    .Handle = (void *)&nor_xspi,
    .ConfigType = EXTMEM_LINK_CONFIG_8LINES,
    .NorSfdpObject =
    {
      {0}
    }
  },
};
#endif /* EXTMEM_C */

/*******************************************************************************/
/*                                 DEBUG TRACE                                 */
/*******************************************************************************/

/* Trace level of each layer; 0 = off. */
#define EXTMEM_DEBUG_LEVEL                 0
#define EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL 0
#define EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL    0
#define EXTMEM_SAL_XSPI_DEBUG_LEVEL        0

#ifdef __cplusplus
}
#endif

#endif /* EXTMEM_CONF_H_ */
