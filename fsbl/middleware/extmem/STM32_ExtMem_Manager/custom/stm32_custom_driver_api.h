/**
  ******************************************************************************
  * @file    stm32_custom_driver_api.h
  * @author  MCD Application Team
  * @brief   This file contains custom driver API definitions.
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
#ifndef __STM32_CUSTOM_DRIVER_H
#define __STM32_CUSTOM_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if EXTMEM_DRIVER_CUSTOM == 1

/** @addtogroup CUSTOM
  * @ingroup EXTMEM_DRIVER
  * @{
  */
/* Exported constants --------------------------------------------------------*/

/* Default EMM Custom Driver timeout: 500ms */
#define CUSTOM_XSPI_TIMEOUT_VALUE   (500)

/* Exported types ------------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Types Exported Types
  * @{
  */

/**
  * @brief List of error codes of the CUSTOM driver
  */
typedef enum
{
  EXTMEM_DRV_CUSTOM_OK                     =  0,          /*!< Status of Operation OK */
  EXTMEM_DRV_CUSTOM_ERR_XSPI_INIT          = -1,          /*!< XSPI initialization error */
  EXTMEM_DRV_CUSTOM_ERR_XSPI_DEINIT        = -2,          /*!< XSPI de-initialization error */
  EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT       = -3,          /*!< XSPI timeout error */
  EXTMEM_DRV_CUSTOM_ERR_CLOCK_INIT         = -4,          /*!< Clock initialization error */
  EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND        = -5,          /*!< Error setting XSPI command */
  EXTMEM_DRV_CUSTOM_ERR_RESET_MEMORY       = -6,          /*!< Error resetting memory */
  EXTMEM_DRV_CUSTOM_ERR_JEDEC_ID           = -7,          /*!< JEDEC ID error */
  EXTMEM_DRV_CUSTOM_ERR_READ_REGISTER      = -8,          /*!< Error reading register */
  EXTMEM_DRV_CUSTOM_ERR_WRITE_REGISTER     = -9,          /*!< Error writing register */
  EXTMEM_DRV_CUSTOM_ERR_SEND_INSTRUCTION   = -10,         /*!< Error sending instruction */
  EXTMEM_DRV_CUSTOM_ERR_WRITE_INSTRUCTION  = -11,         /*!< Error writing instruction */
  EXTMEM_DRV_CUSTOM_ERR_CONFIG_STEP        = -12,         /*!< Configuration step error */
  EXTMEM_DRV_CUSTOM_ERR_MEMORY_TYPE        = -13,         /*!< Memory type error */
  EXTMEM_DRV_CUSTOM_ERR_WRITE_ENABLE       = -14,         /*!< Write enable error */
  EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY         = -15,         /*!< Flash busy error */
  EXTMEM_DRV_CUSTOM_ERR_READ_MEMORY        = -16,         /*!< Error reading memory */
  EXTMEM_DRV_CUSTOM_ERR_WRITE_MEMORY       = -17,         /*!< Error writing memory */
  EXTMEM_DRV_CUSTOM_ERR_ENABLE_MEM_MAP     = -18,         /*!< Error enabling memory-mapped mode */
  EXTMEM_DRV_CUSTOM_ERR_DISABLE_MEM_MAP    = -19,         /*!< Error disabling memory-mapped mode */
  EXTMEM_DRV_CUSTOM_ERR_ADDRESS_ALIGNMENT  = -20,         /*!< Address alignment error */
  EXTMEM_DRV_CUSTOM_ERR_SECTOR_SIZE        = -21,         /*!< Sector size error */
  EXTMEM_DRV_CUSTOM_ERR_ERASE_TIMEOUT      = -22,         /*!< Erase timeout error */
  EXTMEM_DRV_CUSTOM_ERR                    = -128         /*!< Generic error */
} EXTMEM_DRIVER_CUSTOM_StatusTypeDef;

/* Exported functions --------------------------------------------------------*/
/**
  * @addtogroup CUSTOM_Exported_Functions Exported functions
  * @{
  */

EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Init(XSPI_HandleTypeDef *hxspi,  uint32_t ClockInput,
                                                             EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_DeInit(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Read(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                             uint32_t Address, uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Write(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                              uint32_t Address, const uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_EraseSector(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                    uint32_t Address, uint32_t Size);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_MassErase(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Enable_MemoryMappedMode(
  EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Disable_MemoryMappedMode(
  EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_GetMapAddress(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                      uint32_t *BaseAddress);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_GetInfo(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                EXTMEM_DRIVER_CUSTOM_MemInfoTypeDef *MemInfo);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_CUSTOM_DRIVER_H */
