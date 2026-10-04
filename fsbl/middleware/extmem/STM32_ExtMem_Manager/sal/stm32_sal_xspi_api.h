/**
  ******************************************************************************
  * @file    stm32_sal_xspi_api.h
  * @author  MCD Application Team
  * @brief   This file contains SAL XSPI function prototypes.
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
#ifndef __STM32_SAL_XSPI_API_H
#define __STM32_SAL_XSPI_API_H

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SAL_XSPI
  * @ingroup EXTMEM_SAL
  * @{
  */

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup SAL_XSPI_Exported_types SAL XSPI exported types
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SAL_XSPI_Exported_Functions SAL XSPI Exported Functions
  * @{
  */
HAL_StatusTypeDef SAL_XSPI_SetClock(SAL_XSPI_ObjectTypeDef *SalXspi, uint32_t ClockIn, uint32_t ClockRequested,
                                    uint32_t *ClockReal);
HAL_StatusTypeDef SAL_XSPI_Init(SAL_XSPI_ObjectTypeDef *SalXspi, void *HALHandle);
HAL_StatusTypeDef SAL_XSPI_MemoryConfig(SAL_XSPI_ObjectTypeDef *SalXspi, SAL_XSPI_MemParamTypeTypeDef ParametersType,
                                        void *ParamVal);
HAL_StatusTypeDef SAL_XSPI_GetSFDP(SAL_XSPI_ObjectTypeDef *SalXspi, uint32_t Address,
                                   uint8_t *Data, uint32_t DataSize);
HAL_StatusTypeDef SAL_XSPI_GetId(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t *Data, uint32_t DataSize);
HAL_StatusTypeDef SAL_XSPI_Read(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command, uint32_t Address, uint8_t *Data,
                                uint32_t DataSize);
HAL_StatusTypeDef SAL_XSPI_Write(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command, uint32_t Address,
                                 const uint8_t *Data, uint32_t DataSize);
HAL_StatusTypeDef SAL_XSPI_CommandSendAddress(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command, uint32_t Address);
HAL_StatusTypeDef SAL_XSPI_CommandSendData(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command,
                                           uint8_t *Data, uint16_t DataSize);
HAL_StatusTypeDef SAL_XSPI_SendReadCommand(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command,
                                           uint8_t *Data, uint16_t DataSize);
HAL_StatusTypeDef SAL_XSPI_CommandSendReadAddress(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command,
                                                  uint32_t Address, uint8_t *Data, uint16_t DataSize,
                                                  uint8_t ManuId);
HAL_StatusTypeDef SAL_XSPI_CheckStatusRegister(SAL_XSPI_ObjectTypeDef *SalXspi,
                                               uint8_t Command, uint32_t Address, uint8_t MatchValue, uint8_t MatchMask,
                                               uint8_t ManuId, uint32_t Timeout);
HAL_StatusTypeDef SAL_XSPI_ConfigureWrappMode(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t WrapCommand, uint8_t WrapDummy);
HAL_StatusTypeDef SAL_XSPI_EnableMapMode(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t CommandRead, uint8_t DummyRead,
                                         uint8_t CommandWrite, uint8_t DummyWrite);
HAL_StatusTypeDef SAL_XSPI_DisableMapMode(SAL_XSPI_ObjectTypeDef *SalXspi);
HAL_StatusTypeDef SAL_XSPI_UpdateMemoryType(SAL_XSPI_ObjectTypeDef *SalXspi, SAL_XSPI_DataOrderTypeDef DataOrder);

/**
  * @brief This function aborts the transaction
  * @param SalXspi SAL XSPI handle
  * @return @ref HAL_StatusTypeDef
  **/
HAL_StatusTypeDef SAL_XSPI_Abort(SAL_XSPI_ObjectTypeDef *SalXspi);

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

#endif /* __STM32_SAL_XSPI_API_H */

