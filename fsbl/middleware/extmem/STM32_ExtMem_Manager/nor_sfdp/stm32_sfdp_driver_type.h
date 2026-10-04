/**
  ******************************************************************************
  * @file    stm32_sfdp_driver_type.h
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
#ifndef __STM32_SFDP_DRIVER_TYPE_H
#define __STM32_SFDP_DRIVER_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
/** @addtogroup NOR_SFDP
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/**
  * @brief Instruction ID for common JEDEC commands
  */
#define SFDP_DRIVER_READ_COMMAND                             0x03U

#define SFDP_DRIVER_PAGE_PROGRAM_COMMAND                     0x02U

#define SFDP_DRIVER_READ_STATUS_REGISTER_COMMAND             0x05U

#define SFDP_DRIVER_WRITE_ENABLE_50H_COMMAND                 0x50U
#define SFDP_DRIVER_WRITE_ENABLE_06H_COMMAND                 0x06U

#define SFDP_DRIVER_ERASE_CHIP_COMMAND                       0x60U


/* Exported types ------------------------------------------------------------*/

/** @defgroup DRIVER_SFDP_Exported_Types DRIVER SFDP Memory Exported Types
  * @{
  */

/**
  * @brief driver data used to manage the flash
  */
typedef struct
{

  SAL_XSPI_PhysicalLinkTypeDef SpiPhyLink;           /*!< Physical link information */
  uint32_t ClockIn;                                  /*!< Input Clock */

  /* Table of instructions (an instruction equal to zero means not supported) */
  /* WIP : write in progress */
  uint8_t ReadWIPCommand;                            /*!< Read write in progress command */
  uint8_t WIPPosition;                               /*!< Write in progress byte position */
  uint8_t WIPBusyPolarity;                           /*!< 0: Positive (WIP=1 means write is in progress)
                                                          1: Inverted (WIP=0 means write is in progress) */
  uint8_t WIPAddress;                                /*!< Read write in progress address */

  /* WEL: write enable */
  uint8_t WriteWELCommand;                           /*!< Write enable command */

  /* WEL : read write enable */
  uint8_t ReadWELCommand;                            /*!< Read write enable status command */
  uint8_t WELPosition;                               /*!< Write enable position */
  uint8_t WELBusyPolarity;                           /*!< 0: Positive (WEL=1 means write is enabled)
                                                          1: Inverted (WEL=0 means write is enabled) */
  uint8_t WELAddress;                                /*!< write enable address */

  /* Page Program */
  uint8_t PageProgramInstruction;                    /*!< Page program command */

  /* Read management */
  uint8_t ReadInstruction;                           /*!< Read command */

  /* Erase management */
  uint8_t EraseType1Size;                            /*!< Erase 1 size */
  uint8_t EraseType1Command;                         /*!< Erase 1 command */
  uint8_t EraseType2Size;                            /*!< Erase 2 size */
  uint8_t EraseType2Command;                         /*!< Erase 2 command */
  uint8_t EraseType3Size;                            /*!< Erase 3 size */
  uint8_t EraseType3Command;                         /*!< Erase 3 command */
  uint8_t EraseType4Size;                            /*!< Erase 4 size */
  uint8_t EraseType4Command;                         /*!< Erase 4 command */

  uint32_t EraseType1Timing;                         /*!< Erase 1 timing */
  uint32_t EraseType2Timing;                         /*!< Erase 2 timing */
  uint32_t EraseType3Timing;                         /*!< Erase 3 timing */
  uint32_t EraseType4Timing;                         /*!< Erase 4 timing */
  uint32_t EraseChipTiming;                          /*!< Erase chip timing */
} EXTMEM_DRIVER_NOR_SFDP_InfoTypeDef;


/**
  * @brief driver SFDP Object definition
  */
typedef struct
{
  struct
  {
    uint32_t                MaxFreq;              /*!< Maximum frequency supported by the memory.
                                                       @note If the value is zero, the parameter is ignored. */
    uint8_t                 DtrReadDummyCycle;    /*!< Number of dummy cycles for DTR read command.
                                                       @note Used only for JEDEC basic with DTR option. */
  } sfdp_public;
  struct
  {
    SAL_XSPI_ObjectTypeDef    SALObject;             /*!< Instance of the memory */
    EXTMEM_LinkConfig_TypeDef Config;                /*!< Memory configuration */
    uint8_t                   ManuID;                /*!< Manufacturer ID */
    uint8_t                   FlashSize;             /*!< Flash size as a power of two */
    uint32_t                  PageSize;              /*!< Page size */
    EXTMEM_DRIVER_NOR_SFDP_InfoTypeDef  DriverInfo;  /*!< Driver information */
    uint32_t                  Sfdp_table_mask;       /*!< Sfdp table mask */
    uint32_t                  Reset_info;            /*!< Copy of JEDEC Basic 16 Reset/Rescue info */
    uint8_t                   Sfdp_param_number;     /*!< Number of parameters from the SFDP header table */
    uint8_t                   Sfdp_AccessProtocol;   /*!< Access protocol type from the SFDP header table */
  } sfdp_private;
} EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef;

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/**
  * @addtogroup DRIVER_SFDP_Exported_Functions DRIVER SFDP exported functions
  * @{
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

#endif /* __STM32_SFDP_DRIVER_TYPE_H */
