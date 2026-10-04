/**
 * @file     threadx.c
 * @brief    Kernel-started flag shared with threadx_initialize_low_level.S.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

/*******************************************************************************/
/*                               PUBLIC VARIABLES                              */
/*******************************************************************************/

/* SysTick is also used by HAL while bsp_init() is running, before ThreadX
 * calls _tx_initialize_low_level(). */
volatile uint32_t g_threadx_kernel_started;
