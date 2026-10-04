/**
  ******************************************************************************
  * @file    stm32_sfdp_driver_api.h
  * @author  MCD Application Team
  * @brief   This file contains the sfdp driver definition.
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
#ifndef __STM32_NOR_SFDP_DRIVER_API_H
#define __STM32_NOR_SFDP_DRIVER_API_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @addtogroup NOR_SFDP
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/** @defgroup DRIVER_SFDP_Exported_Types DRIVER SFDP Memory Exported Types
  * @{
  */


/**
  * @brief List of error codes for the SFDP driver.
  */
typedef enum
{
  EXTMEM_DRIVER_NOR_SFDP_OK                           =  0,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP                   = -1,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_READ                   = -2,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_BUILD                  = -3,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_UNKNOWN_COMMAND        = -4,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_BUSY                   = -5,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_WRITEENABLE            = -6,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_WRITE                  = -7,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_SECTORTYPE             = -8,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_SECTORTYPE_UNAVAILABLE = -9,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_ADDRESS_ALIGNMENT      = -10,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_ERASE_TIMEOUT          = -11,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_FLASHBUSY              = -12,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_MAP_ENABLE             = -13,
  EXTMEM_DRIVER_NOR_SFDP_ERROR_MEMTYPE_CHECK          = -14,
  EXTMEM_DRIVER_NOR_SFDP_ERROR                        = -128,
} EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef;

/**
  * @brief List of sector types for the SFDP driver.
  */
typedef enum
{
  EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE1,
  EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE2,
  EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE3,
  EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE4
} EXTMEM_DRIVER_NOR_SFDP_SectorTypeTypeDef;

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/**
  * @addtogroup DRIVER_SFDP_Exported_Functions DRIVER SFDP exported functions
  * @{
  */
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Init(void *Peripheral, EXTMEM_LinkConfig_TypeDef Config,
                                                                 uint32_t ClockInput,
                                                                 EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject);
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_DeInit(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject);
void EXTMEM_DRIVER_NOR_SFDP_GetFlashInfo(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject,
                                         EXTMEM_NOR_SFDP_FlashInfoTypeDef *FlashInfo);
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Read(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject,
                                                                 uint32_t Address, uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Write(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject,
                                                                  uint32_t Address, const uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_WriteInMappedMode(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef
                                                                              *SFDPObject, uint32_t Address,
                                                                              const uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_MassErase(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject);
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_SectorErase(
  EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef
  *SFDPObject, uint32_t Address,
  EXTMEM_DRIVER_NOR_SFDP_SectorTypeTypeDef SectorType);
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Enable_MemoryMappedMode(
  EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject);
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Disable_MemoryMappedMode(
  EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_NOR_SFDP_DRIVER_API_H */
