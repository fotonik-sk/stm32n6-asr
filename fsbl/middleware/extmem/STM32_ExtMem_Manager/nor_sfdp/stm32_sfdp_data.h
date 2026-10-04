/**
  ******************************************************************************
  * @file    stm32_sfdp_data.h
  * @author  MCD Application Team
  * @brief   This file contains the sfdp functions prototypes.
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
#ifndef __STM32_SFDP_DATA_H
#define __STM32_SFDP_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if EXTMEM_SAL_XSPI == 1
#include "../sal/stm32_sal_xspi_api.h"
#else
#error "the driver SFDP requires the enable of EXTMEM_SAL_XSPI"
#endif /* EXTMEM_SAL_XSPI_ENABLED */

/**
  * @addtogroup NOR_SFDP_DATA
  * @ingroup NOR_SFDP
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/**
  * @brief Maximum number of SFDP parameter in SFDP table
  */
#define SFDP_MAX_NB_OF_PARAM                                10U

/* Exported types ------------------------------------------------------------*/

/** @defgroup NOR_SFDP_DATA_Exported_Types Exported Types
  * @ingroup NOR_SFDP_DATA
  * @{
  */

/**
  * @brief SFDP parameter table ID
  */
typedef enum
{
  SFDP_PARAMID_UNKNOWN                                      = 0x00000U,
  SFDP_PARAMID_VENDOR                                       = 0x00001U,
  SFDP_PARAMID_FUNCTION_VENDOR                              = 0x00002U,
  SFDP_PARAMID_FUNCTION_JEDEC                               = 0x00004U,
  SFDP_PARAMID_BASIC_SPIPROTOCOL                            = 0x00008U,
  SFDP_PARAMID_SECTORMAP                                    = 0x00010U,
  SFDP_PARAMID_RPMC                                         = 0x00020U,
  SFDP_PARAMID_4BYTE_ADDRESS_INSTRUCTION                    = 0x00040U,
  SFDP_PARAMID_XSPI_V1_0                                    = 0x00080U,
  SFDP_PARAMID_XSPI_V2_0                                    = 0x00100U,
  SFDP_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP           = 0x00200U,
  SFDP_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP_MULTICHIP = 0x00400U,
  SFDP_PARAMID_STATUS_CONTROL_CONFIG_XSPI_V2_0              = 0x00800U,
  SFDP_PARAMID_OCTAL_DDR                                    = 0x01000U,
  SFDP_PARAMID_MSPT                                         = 0x02000U,
  SFDP_PARAMID_X4QUAD_DS                                    = 0x04000U,
  SFDP_PARAMID_QUAD_DDR                                     = 0x08000U,
  SFDP_PARAMID_SECURE_PACKET_READ_WRITE                     = 0x10000U,
  SFDP_PARAMID_RESERVED                                     = 0x20000U
} SFDP_ParamID_TypeDef;

/**
  * @brief SFDP status typedef
  */
typedef enum
{
  EXTMEM_SFDP_OK,                            /*!< Status OK */
  EXTMEM_SFDP_ERROR_PARAM,
  EXTMEM_SFDP_ERROR_NOREADFUNCTION,
  EXTMEM_SFDP_ERROR_SFDPREAD,
  EXTMEM_SFDP_ERROR_SIGNATURE,               /*!< Signature is invalid */
  EXTMEM_SFDP_ERROR_SIGNATUREMTYPE,          /*!< Signature is invalid due to wrong memory type */
  EXTMEM_SFDP_ERROR_PARAMTABLE_NOTFOUND,
  EXTMEM_SFDP_ERROR_NO_PARAMTABLE_BASIC,
  EXTMEM_SFDP_ERROR_NO_32BITADDRESSING,
  EXTMEM_SFDP_ERROR_JEDECBASIC_D14,
  EXTMEM_SFDP_ERROR_JEDECBASIC_D16,
  EXTMEM_SFDP_ERROR_OCTALMODE,
  EXTMEM_SFDP_ERROR_DRIVER,
  EXTMEM_SFDP_ERROR_SETCLOCK,
  EXTMEM_SFDP_ERROR_CONFIGDUMMY,
  EXTMEM_SFDP_ERROR_NOTYETHANDLED
} SFDP_StatusTypeDef;

/**
  * @brief SFDP header
  */
typedef struct
{
  uint32_t Signature;        /*!< Signature value */
  uint8_t  Minor_revision;   /*!< Minor revision */
  uint8_t  Major_revision;   /*!< Major revision */
  uint8_t  param_number;     /*!< Number of parameters */
  uint8_t  AccessProtocol;   /*!< Access protocol type */
} SFDP_HeaderTypeDef;

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup NOR_SFDP_Data_Exported_Functions Exported Functions
  * @{
  */

SFDP_StatusTypeDef SFDP_GetHeader(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object,
                                  SFDP_HeaderTypeDef *sfdp_header);
SFDP_StatusTypeDef SFDP_ReadHeader(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object,
                                   SFDP_HeaderTypeDef *sfdp_header);
SFDP_StatusTypeDef SFDP_CollectData(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object);
SFDP_StatusTypeDef SFDP_MemoryReset(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object);
SFDP_StatusTypeDef SFDP_BuildGenericDriver(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object, uint8_t *FreqUpdated);
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef driver_check_FlagBUSY(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject,
                                                           uint32_t Timeout);


/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_SFDP_DATA_H */
