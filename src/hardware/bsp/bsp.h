/**
 * @file     bsp.h
 * @brief    Board support package entry point (hardware layer).
 * @details  bsp_init() brings the board to the state ThreadX needs. The devices
 *           whose start-up needs the kernel or a fixed position in the startup
 *           sequence are started later by the application: bsp_lcd_init(),
 *           bsp_mic_start() and, for the on-chip NPU, drv_npu_init().
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_BSP_BSP_H
#define HARDWARE_BSP_BSP_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "stm32n6xx_hal.h"
#include "boards.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Bring up the core, HAL and the board devices every thread relies on.
 * @details In order: MPU and caches, HAL (SysTick), the inherited clock tree,
 *          user LEDs, the console UART, the external memories (NOR inherited,
 *          PSRAM mapped). Halts on failure: there is no console to report to.
 * @pre Called first in main(), before the kernel starts.
 */
void bsp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_BSP_BSP_H */
