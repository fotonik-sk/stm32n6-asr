/**
 * @file     drv_npu.h
 * @brief    STM32N6 NPU power, clock and bus-access bring-up.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_DRIVERS_NPU_DRV_NPU_H
#define HARDWARE_DRIVERS_NPU_DRV_NPU_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Power the NPU and its memories and let it reach them.
 */
void drv_npu_init(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_DRIVERS_NPU_DRV_NPU_H */
