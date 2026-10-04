/**
  ******************************************************************************
  * @file    stm32_sal_sd_api.h
  * @author  MCD Application Team
  * @brief   This file contains SAL SD function prototypes.
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
#ifndef __STM32_SAL_SD_API_H
#define __STM32_SAL_SD_API_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SAL_SD
  * @ingroup EXTMEM_SAL
  * @{
  */

/** @defgroup SAL_SD
  * @{
  */

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup SAL_SD_Exported_types SAL SD exported types
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SAL_SD_Exported_Functions SAL SD Exported Functions
  * @{
  */

HAL_StatusTypeDef SAL_SD_Init(SAL_SD_ObjectTypeDef *SalSD, void *HALHandle, EXTMEM_DRIVER_SDCARD_InfoTypeDef *Info);
HAL_StatusTypeDef SAL_SD_DeInit(SAL_SD_ObjectTypeDef *SalSD);
HAL_StatusTypeDef SAL_SD_ReadData(SAL_SD_ObjectTypeDef *SalSD, uint32_t BlockIdx, uint8_t *Data,
                                  uint32_t NumberOfBlock);
HAL_StatusTypeDef SAL_SD_WriteData(SAL_SD_ObjectTypeDef *SalSD, uint32_t BlockIdx, const uint8_t *const Data,
                                   uint32_t NumberOfBlock);
HAL_StatusTypeDef SAL_SD_EraseBlock(SAL_SD_ObjectTypeDef *SalSD, uint32_t BlockIdx, uint32_t BlockCount);
HAL_StatusTypeDef SAL_SD_MassErase(SAL_SD_ObjectTypeDef *SalSD);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_SAL_SD_API_H */
