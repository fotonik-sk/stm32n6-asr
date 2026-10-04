/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * Modifications Copyright (c) 2026 Nikita Prismakov.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef FSBL_HARDWARE_MCU_HAL_MAIN_H
#define FSBL_HARDWARE_MCU_HAL_MAIN_H

/*******************************************************************************/
/*                                  INCLUDES                                   */
/*******************************************************************************/

#include "stm32n6xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/** @brief Signal a boot failure on the red LED and halt; never returns. */
void error_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* FSBL_HARDWARE_MCU_HAL_MAIN_H */
