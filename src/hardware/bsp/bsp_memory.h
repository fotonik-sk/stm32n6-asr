/**
 * @file     bsp_memory.h
 * @brief    Board external memories: XSPI2 NOR (inherited) and XSPI1 PSRAM.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_BSP_BSP_MEMORY_H
#define HARDWARE_BSP_BSP_MEMORY_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "stm32n6xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Make both external memories CPU-addressable.
 * @details The NOR is taken over as the FSBL left it: the FSBL configured XSPI2
 *          and mapped the NOR before jumping into the application, which now
 *          executes in place from it, so reinitializing the controller would
 *          reset the bus underneath the running code. The PSRAM is brought up
 *          here and mapped at BOARD_PSRAM_BASE_ADDR.
 * @return HAL_OK when the PSRAM is mapped (or already was), HAL_ERROR otherwise.
 */
HAL_StatusTypeDef bsp_memory_init(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_BSP_BSP_MEMORY_H */
