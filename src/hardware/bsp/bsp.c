/**
 * @file     bsp.c
 * @brief    Board support package start-up sequence; implements bsp.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "bsp.h"

#include "bsp_console.h"
#include "bsp_led.h"
#include "bsp_memory.h"
#include "mcu_core.h"

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void bsp_init(void)
{
    /* MPU + caches must be configured before any cacheable access. */
    mcu_core_init();

    HAL_Init();

    bsp_led_init();

    if ((bsp_console_init() != 0) || (bsp_memory_init() != HAL_OK))
    {
        /* Halt: nothing can report the failure yet. */
        __disable_irq();
        while (1)
        {
        }
    }
}
