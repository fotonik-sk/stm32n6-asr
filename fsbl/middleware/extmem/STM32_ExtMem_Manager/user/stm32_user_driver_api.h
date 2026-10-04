/**
  ******************************************************************************
  * @file    stm32_user_driver_api.h
  * @author  MCD Application Team
  * @brief   This file contains USER driver API definitions.
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
#ifndef __STM32_USER_DRIVER_API_H
#define __STM32_USER_DRIVER_API_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if EXTMEM_DRIVER_USER == 1

/** @addtogroup USER
  * @ingroup EXTMEM_DRIVER
  * @{
  */
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/** @defgroup USER_Exported_Types Exported Types
  * @{
  */

/**
  * @brief Enumeration of error codes for the USER driver.
  */
typedef enum
{
  EXTMEM_DRIVER_USER_OK        =  0,
  EXTMEM_DRIVER_USER_ERROR_1   = -1,
  EXTMEM_DRIVER_USER_ERROR_2   = -2,
  EXTMEM_DRIVER_USER_ERROR_3   = -3,
  EXTMEM_DRIVER_USER_ERROR_4   = -4,
  EXTMEM_DRIVER_USER_ERROR_5   = -5,
  EXTMEM_DRIVER_USER_ERROR_6   = -6,
  EXTMEM_DRIVER_USER_ERROR_7   = -7,
  EXTMEM_DRIVER_USER_ERROR_8   = -8,
  EXTMEM_DRIVER_USER_ERROR_9   = -9,
  EXTMEM_DRIVER_USER_ERROR_10  = -10,

  EXTMEM_DRIVER_USER_NOTSUPPORTED  = -128
} EXTMEM_DRIVER_USER_StatusTypeDef;

/* Exported functions --------------------------------------------------------*/
/**
  * @addtogroup USER_Exported_Functions Exported functions
  * @{
  */

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Init(uint32_t MemoryId,
                                                         EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject);
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_DeInit(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject);
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Read(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                         uint32_t Address, uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Write(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                          uint32_t Address, const uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_EraseSector(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                                uint32_t Address, uint32_t Size);
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_MassErase(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject);
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Enable_MemoryMappedMode(EXTMEM_DRIVER_USER_ObjectTypeDef
                                                                            *UserObject);
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Disable_MemoryMappedMode(EXTMEM_DRIVER_USER_ObjectTypeDef
                                                                             *UserObject);
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_GetMapAddress(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                                  uint32_t *BaseAddress);
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_GetInfo(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                            EXTMEM_USER_MemInfoTypeDef *MemInfo);

/**
  * @}
  */

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

#endif /* __STM32_USER_DRIVER_API_H */
