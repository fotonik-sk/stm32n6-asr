/**
  ******************************************************************************
  * @file    stm32_user_driver_type.h
  * @author  MCD Application Team
  * @brief   This file contains USER driver type definitions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_USER_DRIVER_TYPE_H
#define __STM32_USER_DRIVER_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/** @addtogroup USER
  * @ingroup EXTMEM_DRIVER
  * @{
  */

#if EXTMEM_DRIVER_USER == 1

/* Exported constants --------------------------------------------------------*/
/** @defgroup USER_Exported_constants Exported constants
  * @{
  */


/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup USER_Exported_Types Exported Types
  * @{
  */

/**
  * @brief USER driver object definition.
  */
typedef struct
{
  uint32_t MemID;           /*!< Memory ID. */
  void *PtrUserDriver;      /*!< Pointer to the user driver instance. */
} EXTMEM_DRIVER_USER_ObjectTypeDef;

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_USER == 1 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_USER_DRIVER_TYPE_H */
