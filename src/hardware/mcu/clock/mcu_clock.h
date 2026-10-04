/**
 * @file     mcu_clock.h
 * @brief    STM32N6 clock tree: system PLL1 (FSBL) and the shared audio PLL3.
 * @details  The one place that programs the PLLs. Peripheral kernel-clock
 *           selections (LTDC IC16, MDF1 IC8, XSPI1) stay with their drivers.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_MCU_CLOCK_MCU_CLOCK_H
#define HARDWARE_MCU_CLOCK_MCU_CLOCK_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Configure the 600 MHz PLL1 system clock tree from HSI.
 * @retval true if configuration completed successfully, otherwise false.
 * @note This is invoked by the FSBL before it enables XIP; the application
 *       inherits the result (HAL_Init() refreshes SystemCoreClock).
 */
bool mcu_clock_init_system_pll(void);

/**
 * @brief Bring up PLL3 at 49.142 MHz from the 64 MHz HSI.
 * @retval true if the PLL locked, otherwise false.
 * @note PLL3 feeds IC8, which drives the MDF1 kernel clock. The FSBL leaves
 *       PLL3 bypassed, so the PDM driver calls this before HAL_MDF_Init(). This
 *       is the divider chain the microphone sample rate depends on; do not
 *       retune it.
 */
bool mcu_clock_enable_audio_pll(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_MCU_CLOCK_MCU_CLOCK_H */
