/**
 * @file     mcu_core.h
 * @brief    Cortex-M55 core setup: MPU regions and the L1 caches.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_MCU_CORE_MCU_CORE_H
#define HARDWARE_MCU_CORE_MCU_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Program the MPU and enable the instruction and data caches.
 * @details Region 0 covers the linker's NONCACHEABLE region and marks it
 *          non-cacheable, so DMA buffers and linked-list nodes placed there need
 *          no cache maintenance. The caches are enabled only after that region
 *          exists.
 * @pre Call first in main(), before any cacheable access to DMA-shared memory.
 */
void mcu_core_init(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_MCU_CORE_MCU_CORE_H */
