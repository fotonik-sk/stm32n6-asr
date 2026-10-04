/**
  ******************************************************************************
  * @file    stm32n6xx_it.c
  * @brief   Interrupt Service Routines.
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

/*******************************************************************************/
/*                                  INCLUDES                                   */
/*******************************************************************************/

#include "stm32n6xx_it.h"

#include "main.h"

/*******************************************************************************/
/*                   CORTEX-M55 EXCEPTION HANDLERS                             */
/*******************************************************************************/

/* The FSBL has no fault reporting: every fault halts where it happened. */

/** @brief Non-maskable interrupt. */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/** @brief Hard fault. */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/** @brief Memory management fault. */
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/** @brief Prefetch fault, memory access fault. */
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/** @brief Undefined instruction or illegal state. */
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/** @brief Secure fault. */
void SecureFault_Handler(void)
{
  while (1)
  {
  }
}

/** @brief System service call via SWI instruction; unused. */
void SVC_Handler(void)
{
}

/** @brief Debug monitor; unused. */
void DebugMon_Handler(void)
{
}

/** @brief Pendable request for system service; unused. */
void PendSV_Handler(void)
{
}

/** @brief System tick timer. */
void SysTick_Handler(void)
{
  HAL_IncTick();
}
