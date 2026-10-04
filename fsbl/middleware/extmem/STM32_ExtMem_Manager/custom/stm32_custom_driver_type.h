/**
  ******************************************************************************
  * @file    stm32_custom_driver_type.h
  * @author  MCD Application Team
  * @brief   This file contains custom driver type definitions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_CUSTOM_TYPE_H
#define __STM32_CUSTOM_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_custom_driver_private_define.h"

/** @addtogroup CUSTOM
  * @ingroup EXTMEM_DRIVER
  * @{
  */

#if EXTMEM_DRIVER_CUSTOM == 1

/*! Macro to check if the type is an access mode type. */
#define IS_ACCESS_MODE_TYPE(_TYPE_)               (((_TYPE_) & ACCESS_MODE_TYPE) == (ACCESS_MODE_TYPE))

/*! Macro to check if the memory type is not flash. */
#define IS_NOT_MEMORY_FLASH(_MEM_)               (((_MEM_) & FLASH_TYPE_MASK) != (FLASH_TYPE_MASK))

/*! Macro to check if Address is present in configuration step. */
#define IS_CFGSTEP_ADDR(_CFGSTEP_)               (((_CFGSTEP_) & CFGSTEP_ADDRESS) == (CFGSTEP_ADDRESS))

/*! Macro to check if configuration step uses 8-bit data size. */
#define IS_CFGSTEP_DATASIZE_8BIT(_CFGSTEP_)      (((_CFGSTEP_) & CFGSTEP_DATASIZE_MASK) == (CFGSTEP_DATASIZE_8BIT))

/*! Macro to check if configuration step uses 16-bit data size. */
#define IS_CFGSTEP_DATASIZE_16BIT(_CFGSTEP_)     (((_CFGSTEP_) & CFGSTEP_DATASIZE_MASK) == (CFGSTEP_DATASIZE_16BIT))

/*!  Macro to check if configuration step is of Instruction type. */
#define IS_CFGSTEP_INST_TYPE(_CFGSTEP_)          (((_CFGSTEP_) & CFGSTEP_CONFIG_TYPE_MASK) == (CFGSTEP_INST_CONFIG))

/*! Macro to check if configuration step is of Status Register access type. */
#define IS_CFGSTEP_REG_ACCESS_TYPE(_CFGSTEP_)    (((_CFGSTEP_) & CFGSTEP_CONFIG_TYPE_MASK) == (CFGSTEP_REG_CONFIG))

/*! Macro to check if configuration step is of Execute Optional config type. */
#define IS_CFGSTEP_EXEC_OPT_CFG_TYPE(_CFGSTEP_)  (((_CFGSTEP_) & CFGSTEP_CONFIG_TYPE_MASK) == (CFGSTEP_EXEC_OPT_CONFIG))

/*! Macro to check if configuration step is basic instruction. */
#define IS_CFGSTEP_INS_BASIC(_CFGSTEP_)          (((_CFGSTEP_) & CFGSTEP_INST_TYPE_MASK) == (CFGSTEP_INST_BASIC))

/*! Macro to check if configuration step is Read register operation. */
#define IS_CFGSTEP_REG_R_ACCESS(_CFGSTEP_)       (((_CFGSTEP_) & CFGSTEP_REG_R_TYPE) == (CFGSTEP_REG_R_TYPE))

/*! Macro to check if configuration step is Write register operation. */
#define IS_CFGSTEP_REG_W_ACCESS(_CFGSTEP_)       (((_CFGSTEP_) & CFGSTEP_REG_W_TYPE) == (CFGSTEP_REG_W_TYPE))

/*! Macro to check if configuration step is ReadAfterWrite register operation. */
#define IS_CFGSTEP_REG_RAW_ACCESS(_CFGSTEP_)     (((_CFGSTEP_) & CFGSTEP_REG_RAW_TYPE) == (CFGSTEP_REG_RAW_TYPE))

/*!f Macro to check if setting is defined. */
#define IS_CUSTOM_SETTING_CFG_DEFINED(SETTING)   (((SETTING) &\
                                                   CUSTOM_SETTING_CFG_DEFINED) == CUSTOM_SETTING_CFG_DEFINED)
/*!  Macro to get setting value. */
#define CUSTOM_SETTING_CFG_VALUE(SETTING)        ((SETTING) & CUSTOM_SETTING_CFG_MASK)

/* Exported constants --------------------------------------------------------*/
/** @defgroup CUSTOM_Exported_constants Exported constants
  * @{
  */

/*!  Maximum number of register configurations. */
#define EXTMEM_CUSTOM_MAX_REG_CONFIG         6U

/*!  Read JEDEC ID command value. */
#define EXTMEM_CUSTOM_JEDECID_CMD            0x9F

/*! Size of JEDEC ID. */
#define EXTMEM_CUSTOM_JEDECID_SIZE           3U

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup CUSTOM_Exported_Types Exported Types
  * @{
  */

/**
  * @brief Enumeration of supported memory sizes.
  */
typedef enum
{
  EXTMEM_CUSTOM_SIZE_16B    = HAL_XSPI_SIZE_16B,                                  /*!<  16 bits  (  2  Bytes = 2^( 0+1)) */
  EXTMEM_CUSTOM_SIZE_32B    = HAL_XSPI_SIZE_32B,                                  /*!<  32 bits  (  4  Bytes = 2^( 1+1)) */
  EXTMEM_CUSTOM_SIZE_64B    = HAL_XSPI_SIZE_64B,                                  /*!<  64 bits  (  8  Bytes = 2^( 2+1)) */
  EXTMEM_CUSTOM_SIZE_128B   = HAL_XSPI_SIZE_128B,                                 /*!< 128 bits  ( 16  Bytes = 2^( 3+1)) */
  EXTMEM_CUSTOM_SIZE_256B   = HAL_XSPI_SIZE_256B,                                 /*!< 256 bits  ( 32  Bytes = 2^( 4+1)) */
  EXTMEM_CUSTOM_SIZE_512B   = HAL_XSPI_SIZE_512B,                                 /*!< 512 bits  ( 64  Bytes = 2^( 5+1)) */
  EXTMEM_CUSTOM_SIZE_1KB    = HAL_XSPI_SIZE_1KB,                                  /*!<   1 Kbits (128  Bytes = 2^( 6+1)) */
  EXTMEM_CUSTOM_SIZE_2KB    = HAL_XSPI_SIZE_2KB,                                  /*!<   2 Kbits (256  Bytes = 2^( 7+1)) */
  EXTMEM_CUSTOM_SIZE_4KB    = HAL_XSPI_SIZE_4KB,                                  /*!<   4 Kbits (512  Bytes = 2^( 8+1)) */
  EXTMEM_CUSTOM_SIZE_8KB    = HAL_XSPI_SIZE_8KB,                                  /*!<   8 Kbits (  1 KBytes = 2^( 9+1)) */
  EXTMEM_CUSTOM_SIZE_16KB   = HAL_XSPI_SIZE_16KB,                                 /*!<  16 Kbits (  2 KBytes = 2^(10+1)) */
  EXTMEM_CUSTOM_SIZE_32KB   = HAL_XSPI_SIZE_32KB,                                 /*!<  32 Kbits (  4 KBytes = 2^(11+1)) */
  EXTMEM_CUSTOM_SIZE_64KB   = HAL_XSPI_SIZE_64KB,                                 /*!<  64 Kbits (  8 KBytes = 2^(12+1)) */
  EXTMEM_CUSTOM_SIZE_128KB  = HAL_XSPI_SIZE_128KB,                                /*!< 128 Kbits ( 16 KBytes = 2^(13+1)) */
  EXTMEM_CUSTOM_SIZE_256KB  = HAL_XSPI_SIZE_256KB,                                /*!< 256 Kbits ( 32 KBytes = 2^(14+1)) */
  EXTMEM_CUSTOM_SIZE_512KB  = HAL_XSPI_SIZE_512KB,                                /*!< 512 Kbits ( 64 KBytes = 2^(15+1)) */
  EXTMEM_CUSTOM_SIZE_1MB    = HAL_XSPI_SIZE_1MB,                                  /*!<   1 Mbits (128 KBytes = 2^(16+1)) */
  EXTMEM_CUSTOM_SIZE_2MB    = HAL_XSPI_SIZE_2MB,                                  /*!<   2 Mbits (256 KBytes = 2^(17+1)) */
  EXTMEM_CUSTOM_SIZE_4MB    = HAL_XSPI_SIZE_4MB,                                  /*!<   4 Mbits (512 KBytes = 2^(18+1)) */
  EXTMEM_CUSTOM_SIZE_8MB    = HAL_XSPI_SIZE_8MB,                                  /*!<   8 Mbits (  1 MBytes = 2^(19+1)) */
  EXTMEM_CUSTOM_SIZE_16MB   = HAL_XSPI_SIZE_16MB,                                 /*!<  16 Mbits (  2 MBytes = 2^(20+1)) */
  EXTMEM_CUSTOM_SIZE_32MB   = HAL_XSPI_SIZE_32MB,                                 /*!<  32 Mbits (  4 MBytes = 2^(21+1)) */
  EXTMEM_CUSTOM_SIZE_64MB   = HAL_XSPI_SIZE_64MB,                                 /*!<  64 Mbits (  8 MBytes = 2^(22+1)) */
  EXTMEM_CUSTOM_SIZE_128MB  = HAL_XSPI_SIZE_128MB,                                /*!< 128 Mbits ( 16 MBytes = 2^(23+1)) */
  EXTMEM_CUSTOM_SIZE_256MB  = HAL_XSPI_SIZE_256MB,                                /*!< 256 Mbits ( 32 MBytes = 2^(24+1)) */
  EXTMEM_CUSTOM_SIZE_512MB  = HAL_XSPI_SIZE_512MB,                                /*!< 512 Mbits ( 64 MBytes = 2^(25+1)) */
  EXTMEM_CUSTOM_SIZE_1GB    = HAL_XSPI_SIZE_1GB,                                  /*!<   1 Gbits (128 MBytes = 2^(26+1)) */
  EXTMEM_CUSTOM_SIZE_2GB    = HAL_XSPI_SIZE_2GB,                                  /*!<   2 Gbits (256 MBytes = 2^(27+1)) */
  EXTMEM_CUSTOM_SIZE_4GB    = HAL_XSPI_SIZE_4GB,                                  /*!<   4 Gbits (512 MBytes = 2^(28+1)) */
  EXTMEM_CUSTOM_SIZE_8GB    = HAL_XSPI_SIZE_8GB,                                  /*!<   8 Gbits (  1 GBytes = 2^(29+1)) */
  EXTMEM_CUSTOM_SIZE_16GB   = HAL_XSPI_SIZE_16GB,                                 /*!<  16 Gbits (  2 GBytes = 2^(30+1)) */
  EXTMEM_CUSTOM_SIZE_32GB   = HAL_XSPI_SIZE_32GB                                  /*!<  32 Gbits (  4 GBytes = 2^(31+1)) */
} EXTMEM_CUSTOM_MemSizeTypeDef;

#if defined (HAL_XSPI_CSSEL_NCS1)
/**
  * @brief Enumeration of nCS configurations.
  */
typedef enum
{
  EXTMEM_CUSTOM_NCS1 = HAL_XSPI_CSSEL_NCS1, /*!< The output of nCS is nCS1 */
  EXTMEM_CUSTOM_NCS2 = HAL_XSPI_CSSEL_NCS2  /*!< The output of nCS is nCS2 */
} EXTMEM_CUSTOM_MemNcsTypeDef;
#endif /* HAL_XSPI_CSSEL_NCS1 */

/**
  * @brief Enumeration of combined access mode configurations.
  */
typedef enum
{
  EXTMEM_CUSTOM_1S_1S_1S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1S | DATA_1S),      /*!< 1S instruction, 1S address, 1S data  */
  EXTMEM_CUSTOM_1S_1S_1D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1S | DATA_1D),      /*!< 1S instruction, 1S address, 1D data  */
  EXTMEM_CUSTOM_1S_1D_1S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1D | DATA_1S),      /*!< 1S instruction, 1D address, 1S data  */
  EXTMEM_CUSTOM_1S_1D_1D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1D | DATA_1D),      /*!< 1S instruction, 1D address, 1D data  */
  EXTMEM_CUSTOM_1D_1S_1S  = (ACCESS_MODE_TYPE | INSTRUCTION_1D | ADDRESS_1S | DATA_1S),      /*!< 1D instruction, 1S address, 1S data  */
  EXTMEM_CUSTOM_1D_1S_1D  = (ACCESS_MODE_TYPE | INSTRUCTION_1D | ADDRESS_1S | DATA_1D),      /*!< 1D instruction, 1S address, 1D data  */
  EXTMEM_CUSTOM_1D_1D_1S  = (ACCESS_MODE_TYPE | INSTRUCTION_1D | ADDRESS_1D | DATA_1S),      /*!< 1D instruction, 1D address, 1S data  */
  EXTMEM_CUSTOM_1D_1D_1D  = (ACCESS_MODE_TYPE | INSTRUCTION_1D | ADDRESS_1D | DATA_1D),      /*!< 1D instruction, 1D address, 1D data  */
  EXTMEM_CUSTOM_1S_1S_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1S | DATA_4S),      /*!< 1S instruction, 1S address, 4S data  */
  EXTMEM_CUSTOM_1S_1S_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1S | DATA_4D),      /*!< 1S instruction, 1S address, 4D data  */
  EXTMEM_CUSTOM_1S_1D_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1D | DATA_4S),      /*!< 1S instruction, 1D address, 4S data  */
  EXTMEM_CUSTOM_1S_1D_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1D | DATA_4D),      /*!< 1S instruction, 1D address, 4D data  */
  EXTMEM_CUSTOM_1S_4S_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_4S | DATA_4S),      /*!< 1S instruction, 4S address, 4S data  */
  EXTMEM_CUSTOM_1S_4S_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_4S | DATA_4D),      /*!< 1S instruction, 4S address, 4D data  */
  EXTMEM_CUSTOM_1S_4D_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_4D | DATA_4S),      /*!< 1S instruction, 4D address, 4S data  */
  EXTMEM_CUSTOM_1S_4D_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_4D | DATA_4D),      /*!< 1S instruction, 4D address, 4D data  */
  EXTMEM_CUSTOM_4S_4S_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_4S | ADDRESS_4S | DATA_4S),      /*!< 4S instruction, 4S address, 4S data  */
  EXTMEM_CUSTOM_4S_4S_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_4S | ADDRESS_4S | DATA_4D),      /*!< 4S instruction, 4S address, 4D data  */
  EXTMEM_CUSTOM_4S_4D_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_4S | ADDRESS_4D | DATA_4S),      /*!< 4S instruction, 4D address, 4S data  */
  EXTMEM_CUSTOM_4S_4D_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_4S | ADDRESS_4D | DATA_4D),      /*!< 4S instruction, 4D address, 4D data  */
  EXTMEM_CUSTOM_4D_4S_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_4D | ADDRESS_4S | DATA_4S),      /*!< 4D instruction, 4S address, 4S data  */
  EXTMEM_CUSTOM_4D_4S_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_4D | ADDRESS_4S | DATA_4D),      /*!< 4D instruction, 4S address, 4D data  */
  EXTMEM_CUSTOM_4D_4D_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_4D | ADDRESS_4D | DATA_4S),      /*!< 4D instruction, 4D address, 4S data  */
  EXTMEM_CUSTOM_4D_4D_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_4D | ADDRESS_4D | DATA_4D),      /*!< 4D instruction, 4D address, 4D data  */
  EXTMEM_CUSTOM_8S_8S_8S  = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8S | DATA_8S),      /*!< 8S instruction, 8S address, 8S data  */
  EXTMEM_CUSTOM_8S_8S_8D  = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8S | DATA_8D),      /*!< 8S instruction, 8S address, 8D data  */
  EXTMEM_CUSTOM_8S_8D_8S  = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8D | DATA_8S),      /*!< 8S instruction, 8D address, 8S data  */
  EXTMEM_CUSTOM_8S_8D_8D  = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8D | DATA_8D),      /*!< 8S instruction, 8D address, 8D data  */
  EXTMEM_CUSTOM_8D_8S_8S  = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8S | DATA_8S),      /*!< 8D instruction, 8S address, 8S data  */
  EXTMEM_CUSTOM_8D_8S_8D  = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8S | DATA_8D),      /*!< 8D instruction, 8S address, 8D data  */
  EXTMEM_CUSTOM_8D_8D_8S  = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8D | DATA_8S),      /*!< 8D instruction, 8D address, 8S data  */
  EXTMEM_CUSTOM_8D_8D_8D  = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8D | DATA_8D),      /*!< 8D instruction, 8D address, 8D data  */
#if defined (HAL_XSPI_DATA_16_LINES)
  EXTMEM_CUSTOM_8S_8D_16D = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8D | DATA_16D),      /*!< 8S instruction, 8D address, 16D data */
  EXTMEM_CUSTOM_8D_8D_16D = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8D | DATA_16D),      /*!< 8D instruction, 8D address, 16D data */
#endif /* HAL_XSPI_DATA_16_LINES */
} EXTMEM_CUSTOM_AccessModeTypeDef;

/**
  * @brief Enumeration of instruction bit sizes.
  */
typedef enum
{
  EXTMEM_CUSTOM_INSTRUCTION_8_BITS  = HAL_XSPI_INSTRUCTION_8_BITS,                /*!< 8-bit instruction  */
  EXTMEM_CUSTOM_INSTRUCTION_16_BITS = HAL_XSPI_INSTRUCTION_16_BITS,               /*!< 16-bit instruction */
  EXTMEM_CUSTOM_INSTRUCTION_24_BITS = HAL_XSPI_INSTRUCTION_24_BITS,               /*!< 24-bit instruction */
  EXTMEM_CUSTOM_INSTRUCTION_32_BITS = HAL_XSPI_INSTRUCTION_32_BITS                /*!< 32-bit instruction */
} EXTMEM_CUSTOM_InstruSizeTypeDef;

/**
  * @brief Enumeration of address bit sizes.
  */
typedef enum
{
  EXTMEM_CUSTOM_ADDRESS_8_BITS  = HAL_XSPI_ADDRESS_8_BITS,                        /*!< 8-bit address  */
  EXTMEM_CUSTOM_ADDRESS_16_BITS = HAL_XSPI_ADDRESS_16_BITS,                       /*!< 16-bit address */
  EXTMEM_CUSTOM_ADDRESS_24_BITS = HAL_XSPI_ADDRESS_24_BITS,                       /*!< 24-bit address */
  EXTMEM_CUSTOM_ADDRESS_32_BITS = HAL_XSPI_ADDRESS_32_BITS                        /*!< 32-bit address */
} EXTMEM_CUSTOM_AddrSizeTypeDef;

/**
  * @brief Enumeration of DQS configurations.
  */
typedef enum
{
  EXTMEM_CUSTOM_DQS_DISABLE = HAL_XSPI_DQS_DISABLE,                               /*!< DQS disabled */
  EXTMEM_CUSTOM_DQS_ENABLE  = HAL_XSPI_DQS_ENABLE                                 /*!< DQS enabled  */
} EXTMEM_CUSTOM_DqsModeTypeDef;

/**
  * @brief Enumeration of supported memory types.
  */
typedef enum
{
  EXTMEM_CUSTOM_NOR_FLASH    = (CUSTOM_NOR_FLASH  | FLASH_TYPE_MASK),             /*!< NOR Flash memory type  */
  EXTMEM_CUSTOM_PSRAM        = (CUSTOM_PSRAM)                                     /*!< PSRAM memory type      */
} EXTMEM_CUSTOM_MemTypeTypeDef;

/**
  * @brief Enumeration of Sample Shift configurations.
  */
typedef enum
{
  EXTMEM_CUSTOM_SSHIFT_CFG_UNDEFINED = CUSTOM_SETTING_CFG_UNDEFINED,                                 /*!< Sample Shift setting not provided in configuration file */
  EXTMEM_CUSTOM_SSHIFT_CFG_NONE      = CUSTOM_SETTING_CFG_DEFINED | HAL_XSPI_SAMPLE_SHIFT_NONE,      /*!< Sample Shift setting provided in configuration file, and to be set to None */
  EXTMEM_CUSTOM_SSHIFT_CFG_HALFCYCLE = CUSTOM_SETTING_CFG_DEFINED | HAL_XSPI_SAMPLE_SHIFT_HALFCYCLE  /*!< Sample Shift setting provided in configuration file, and to be set to Half-cycle */
} EXTMEM_CUSTOM_SShiftCfgTypeDef;

/**
  * @brief Enumeration of supported memory standards.
  */
typedef enum
{
  EXTMEM_CUSTOM_STD_MICRON       = HAL_XSPI_MEMTYPE_MICRON,                       /*!< Micron memory standard         */
  EXTMEM_CUSTOM_STD_MACRONIX     = HAL_XSPI_MEMTYPE_MACRONIX,                     /*!< Macronix memory standard       */
  EXTMEM_CUSTOM_STD_APMEM        = HAL_XSPI_MEMTYPE_APMEM,                        /*!< AP Memory standard             */
  EXTMEM_CUSTOM_STD_MACRONIX_RAM = HAL_XSPI_MEMTYPE_MACRONIX_RAM,                 /*!< Macronix RAM memory standard   */
  EXTMEM_CUSTOM_STD_APMEM_16BITS = HAL_XSPI_MEMTYPE_APMEM_16BITS                  /*!< AP Memory 16-bit standard      */
} EXTMEM_CUSTOM_MemStdTypeDef;

/**
  * @brief Enumeration of possible configuration step types for memory operations.
  * @note Register configuration can be an RWConfigStep or an INSConfigStep structure,
  *       depending on the ConfigStepType.
  * @note In these structures, some fields are applicable while others are not. The comment
  *       for each ConfigStepType describes the list of required fields.
  * @note For RWConfigStep, the fields available for configuration include:
  *       - RegisterConfig[n].RWConfigStep.CommandRegisterWrite: Command to write a register.
  *       - RegisterConfig[n].RWConfigStep.CommandRegisterRead: Command to read a register.
  *       - RegisterConfig[n].RWConfigStep.RegisterAddress: Address of the register.
  *       - RegisterConfig[n].RWConfigStep.RegisterValue: Value to write inside register (masked with RegisterMask).
  *       - RegisterConfig[n].RWConfigStep.RegisterMask: Mask applied to the value before writing to the register.
  * @note For INSConfigStep, the fields available for configuration include:
  *       - RegisterConfig[n].INSConfigStep.Instruction: Instruction sent to the memory.
  */
typedef enum
{
  EXTMEM_CUSTOM_CFGSTEP_UNDEFINED          = 0,                                                            /*!< Configuration step not defined                             */
  EXTMEM_CUSTOM_CFGSTEP_RW_REG_ADDR        = (CUSTOM_CFGSTEP_RW_REGISTER | CFGSTEP_ADDRESS),               /*!< Read then Write register operation with address.           Mandatory fields: CommandRegisterWrite,  CommandRegisterRead, RegisterAddress,       RegisterValue,   RegisterMask                */
  EXTMEM_CUSTOM_CFGSTEP_RW_REG             = (CUSTOM_CFGSTEP_RW_REGISTER),                                 /*!< Read then Write register operation (no address)            Mandatory fields: CommandRegisterWrite,  CommandRegisterRead, RegisterValue,         RegisterMask                                 */
  EXTMEM_CUSTOM_CFGSTEP_RWR_REG_ADDR       = (CUSTOM_CFGSTEP_RWR_REGISTER | CFGSTEP_ADDRESS),              /*!< Read, Write then Read register operation with address      Mandatory fields: CommandRegisterWrite,  CommandRegisterRead, CommandRegisterReadAW, RegisterAddress, RegisterValue, RegisterMask */
  EXTMEM_CUSTOM_CFGSTEP_RWR_REG            = (CUSTOM_CFGSTEP_RWR_REGISTER),                                /*!< Read, Write then Read register operation (no address)      Mandatory fields: CommandRegisterWrite,  CommandRegisterRead, CommandRegisterReadAW, RegisterValue,   RegisterMask                */
  EXTMEM_CUSTOM_CFGSTEP_R_REG_ADDR         = (CUSTOM_CFGSTEP_R_REGISTER | CFGSTEP_ADDRESS),                /*!< Read register operation with address                       Mandatory fields: CommandRegisterRead,   RegisterAddress                                                                          */
  EXTMEM_CUSTOM_CFGSTEP_R_REG              = (CUSTOM_CFGSTEP_R_REGISTER),                                  /*!< Read register operation (no address)                       Mandatory fields: CommandRegisterRead,                                                                                            */
  EXTMEM_CUSTOM_CFGSTEP_W_REG_ADDR         = (CUSTOM_CFGSTEP_W_REGISTER | CFGSTEP_ADDRESS),                /*!< Write register operation with address                      Mandatory fields: CommandRegisterWrite,  RegisterAddress                                                                          */
  EXTMEM_CUSTOM_CFGSTEP_W_REG              = (CUSTOM_CFGSTEP_W_REGISTER),                                  /*!< Write register operation (no address)                      Mandatory fields: CommandRegisterWrite                                                                                            */
  EXTMEM_CUSTOM_CFGSTEP_RAW_REG_ADDR       = (CUSTOM_CFGSTEP_RAW_REGISTER | CFGSTEP_ADDRESS),              /*!< Read register after a Write (wait WIP) with address        Mandatory fields: CommandRegisterReadAW, RegisterAddress                                                                          */
  EXTMEM_CUSTOM_CFGSTEP_RAW_REG            = (CUSTOM_CFGSTEP_RAW_REGISTER),                                /*!< Read register after a Write (wait WIP) (no address)        Mandatory fields: CommandRegisterReadAW                                                                                           */
  EXTMEM_CUSTOM_CFGSTEP_WW_REG_ADDR        = (CUSTOM_CFGSTEP_WW_REGISTER | CFGSTEP_ADDRESS),               /*!< Write 2 bytes (or 16bits) register operation with address  Mandatory fields: CommandRegisterWrite,  RegisterAddress, RegisterValue, RegisterMask                                             */
  EXTMEM_CUSTOM_CFGSTEP_WW_REG             = (CUSTOM_CFGSTEP_WW_REGISTER),                                 /*!< Write 2 bytes (or 16bits) register operation (no address)  Mandatory fields: CommandRegisterWrite,  RegisterValue,   RegisterMask                                                            */
  EXTMEM_CUSTOM_CFGSTEP_INST               = (CUSTOM_CFGSTEP_BASIC_INSTRUCTION),                           /*!< Instruction operation (no address, no data)                Mandatory fields: Instruction                                                                                                     */
  EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG       = (CUSTOM_CFGSTEP_EXEC_OPT_CFG)                                 /*!< Execute Optional configuration (New Freq, AccessMode, etc.)*/

} ConfigStepTypeDef;

/**
  * @brief Enumeration of possible reset methods defined in the configuration file.
  */
typedef enum
{
  EXTMEM_CUSTOM_RESET_METHOD_UNDEFINED = 0,                                       /*!< Reset method is not used/defined in configuration file */
  EXTMEM_CUSTOM_RESET_METHOD_66_99,                                               /*!< Reset method using 66h and 99h instructions */
  EXTMEM_CUSTOM_RESET_METHOD_6699_9966,                                           /*!< Reset method using 6699h and 9966h instructions */
  EXTMEM_CUSTOM_RESET_METHOD_6666_9999,                                           /*!< Reset method using 6666h and 9999h instructions */
  EXTMEM_CUSTOM_RESET_METHOD_FF,                                                  /*!< Reset method using FFh instructions */
} EXTMEM_CUSTOM_ResetMethodTypeDef;

/**
  * @brief Enumeration of details describing access to register containing WIP and WEL bits.
  */
typedef enum
{
  EXTMEM_CUSTOM_SR_ACCESS_CFG_UNDEFINED = CUSTOM_SETTING_CFG_UNDEFINED,                        /*!< Setting undefined (no address required) */
  EXTMEM_CUSTOM_SR_ACCESS_ADD_0         = CUSTOM_SETTING_CFG_DEFINED | CUSTOM_SR_ACCESS_ADD_0, /*!< Address required (address value stored on bits 0-30) */
} EXTMEM_CUSTOM_SRAccessDetails;

/**
  * @brief Structure defining the base memory configuration.
  */
typedef struct
{
  uint32_t                         Frequency;                                     /*!< Operating frequency */
  uint16_t                         CommandRead;                                   /*!< Command to read data */
  uint16_t                         CommandWrite;                                  /*!< Command to write data */
  uint16_t                         DummyCycleRead;                                /*!< Number of dummy cycles for read operations */
  uint16_t                         DummyCycleWrite;                               /*!< Number of dummy cycles for write operations */
  uint16_t                         DummyRegisterRead;                             /*!< Number of dummy cycles for Read Reg operations */
  EXTMEM_CUSTOM_AccessModeTypeDef  AccessMode;                                    /*!< Access mode (e.g., 1S4D4D) */
  EXTMEM_CUSTOM_AccessModeTypeDef  OptWriteAccessMode;                            /*!< Optional Access mode for Write operation (e.g., 1S4D4D) */
  EXTMEM_CUSTOM_InstruSizeTypeDef  InstructionSize;                               /*!< Size of the instruction */
  EXTMEM_CUSTOM_AddrSizeTypeDef    AddressSize;                                   /*!< Size of the address */
  EXTMEM_CUSTOM_DqsModeTypeDef     DqsMode;                                       /*!< Data strobe mode */
} MemoryBaseConfigTypeDef;


/**
  * @brief Structure defining optional memory configuration parameters.
  */
typedef struct
{
  uint32_t                         AlternateByteMode;                             /*!< Alternate byte mode (e.g., 1S, 4D) */
  uint32_t                         AlternateByteSize;                             /*!< Size of the alternate byte */
  uint32_t                         SampleShift;                                   /*!< 1/2 Cycle SDR */
  uint32_t                         Dhqc;                                          /*!< 1/4 Cycle (DTR) maybe STR */
} MemoryOptionalConfigTypeDef;


typedef struct
{
  EXTMEM_CUSTOM_SRAccessDetails    SRAccessDetails;                               /*!< Details about how to access SR for checking WIP, WEL bits */
  uint16_t                         Cmd_EraseSector;                               /*!< Command to erase a sector */
  uint16_t                         Cmd_MassErase;                                 /*!< Command to perform mass erase */
  uint16_t                         Cmd_RDSR;                                      /*!< Command to read status register */
  uint16_t                         Cmd_WE;                                        /*!< Command to enable write operations */
  uint8_t                          MatchMask_WEL;                                 /*!< Mask for write enable latch */
  uint8_t                          MatchValue_WEL;                                /*!< Match Value for write enable latch */
  uint8_t                          MatchMask_WIP;                                 /*!< Mask for write in progress */
  uint8_t                          MatchValue_WIP;                                /*!< Match Value for write in progress */
} NorFlashBasConfigTypeDef;


/**
  * @brief Define the NOR configuration structure
  */
typedef struct
{
  uint32_t                         PageSize;                                      /*!< Page Size (Max write) */
  uint32_t                         SectorSize;                                    /*!< Sector Size in bytes  */
  uint32_t                         MaxSectorEraseTime;                            /*!< Max time expected for completing a Sector Erase operation (in ms) */
  uint32_t                         MaxChipEraseTime;                              /*!< Max time expected for completing a Chip Erase operation (in ms) */
  NorFlashBasConfigTypeDef         Startup;                                       /*!< Config use after boot */
  uint8_t                          OptionalConfigEnable;                          /*!< Optional NorFlash config activation if different from boot (RDSR/WE/Erase commands)  */
  NorFlashBasConfigTypeDef         Optional;                                      /*!< Config use after memory setup if different from boot (RDSR/WE/Erase commands) */
} NorFlashConfigTypeDef;

/**
  * @brief Define the RAM configuration structure
  */
typedef struct
{
  uint32_t                         WrapSize;                                      /*!< Wrap size */
  uint32_t                         RowBoundarySize;                               /*!< Row boundary size */
} RamConfigTypeDef;

/**
  * @brief Define the optional register configuration structure for RW register
  */
typedef struct
{
  uint32_t                         RegisterAddress;                               /*!< Address of the register */
  uint16_t                         CommandRegisterWrite;                          /*!< Command to write to a register */
  uint16_t                         CommandRegisterRead;                           /*!< Command to read from a register */
  uint16_t                         CommandRegisterReadAW;                         /*!< Command to read from a register after write operation (only used in RWR operation type) */
  uint16_t                         RegisterValue;                                 /*!< Value of the register */
  uint16_t                         RegisterExpectedValue;                         /*!< Value expected of the register. Used for comparison in RAW operation */
  uint16_t                         RegisterMask;                                  /*!< Mask for the register */
} RegAccess_TypeDef;

/**
  * @brief Define the optional register configuration structure for Additional Instruction sending
  */
typedef struct
{
  uint16_t                         Instruction;                                   /*!< Instruction to be sent */
} Inst_TypeDef;

/**
  * @brief Define the optional register configuration and additional instructions structure
  */
typedef struct
{
  ConfigStepTypeDef                ConfigStepType;                                /*!< Type of configuration step : Status Register access, Instruction, ...  */
  union
  {
    RegAccess_TypeDef              RWConfigStep;                                  /*!< Config Step of type Register Access (R, W, RW, ...) */
    Inst_TypeDef                   INSConfigStep;                                 /*!< Config Step of type Instruction */
  };
} OptionalRegisterConfigTypeDef;

/**
  * @brief CUSTOM Driver JedecID Info structure definition
  */
typedef struct
{
  uint8_t                          ManufacturerID;                                /*!< JedecID (Manufacturer ID) */
  uint8_t                          MemoryType;                                    /*!< JedecID (Memory type)     */
  uint8_t                          MemoryDensity;                                 /*!< JedecID (Memory density)  */
} EXTMEM_CUSTOM_JedecIDTypeDef;

/**
  * @brief Define the optional register configuration structure
  */
typedef struct
{
  XSPI_HandleTypeDef               *Handle;                                       /*!< Pointer to the XSPI instance handle. */
  XSPI_RegularCmdTypeDef           Commandbase;                                   /*!< Command base configuration. */
  uint32_t                         RealClock;                                     /*!< Actual clock used by the memory. */
  uint32_t                         ReadRegValue;                                  /*!< Value read from register. */
  NorFlashBasConfigTypeDef         *CurrentFlashConfig;                           /*!< Pointer to the current Flash configuration. */
  EXTMEM_CUSTOM_JedecIDTypeDef     JedecID;                                       /*!< JEDEC ID retrieved from memory. */
} CustomPrivateTypeDef;

/**
  * @brief CUSTOM Driver object definition
  */
typedef struct
{
  CustomPrivateTypeDef             Private;                                       /*!< Private driver data. */
  EXTMEM_CUSTOM_MemTypeTypeDef     MemType;                                       /*!< Memory type. */
  EXTMEM_CUSTOM_MemStdTypeDef      MemStandard;                                   /*!< Memory standard (e.g., Macronix, Micron, etc.). */
  EXTMEM_CUSTOM_MemSizeTypeDef     MemSize;                                       /*!< Memory size. */
  EXTMEM_CUSTOM_ResetMethodTypeDef ResetMethod;                                   /*!< Reset method. */
  EXTMEM_CUSTOM_SShiftCfgTypeDef   SampleShiftCfg;                                /*!< Sample Shift configuration. */
  uint16_t                         ResetDelay;                                    /*!< Time needed after reset (in ms, 0 if no required delay). */
  uint16_t                         MemChipSelectHighTimeCycle;                    /*!< Chip select high time. */
  MemoryBaseConfigTypeDef          StartupConfig;                                 /*!< Startup configuration. */
  uint16_t                         NbRegisterConfig;                              /*!< Number of additional configuration commands or instructions to send to the memory. */
  OptionalRegisterConfigTypeDef    RegisterConfig[EXTMEM_CUSTOM_MAX_REG_CONFIG];  /*!< Additional configuration commands or instructions to send to the memory. */
  MemoryBaseConfigTypeDef          OptionalConfig;                                /*!< Optional configuration. */
  union
  {
    NorFlashConfigTypeDef          NorFlashConfig;                                /*!< NOR Flash configuration             */
    RamConfigTypeDef               PsramConfig;                                   /*!< PSRAM configuration                 */
  };
} EXTMEM_DRIVER_CUSTOM_ObjectTypeDef;

/**
  * @brief CUSTOM Driver Memory Info structure definition
  */
typedef struct
{
  EXTMEM_CUSTOM_MemTypeTypeDef     MemType;                                       /*!< Memory type. */
  EXTMEM_CUSTOM_MemSizeTypeDef     MemSize;                                       /*!< Memory size. */
  EXTMEM_CUSTOM_MemStdTypeDef      MemStandard;                                   /*!< Memory standard (e.g., Macronix, Micron, etc.). */
  EXTMEM_CUSTOM_AccessModeTypeDef  MemAccessMode;                                 /*!< Access mode (e.g., 1S4D4D). */
  uint32_t                         MemRealFreq;                                   /*!< Actual operating frequency. */
  EXTMEM_CUSTOM_JedecIDTypeDef     JedecID;                                       /*!< JEDEC ID retrieved from memory. */
} EXTMEM_DRIVER_CUSTOM_MemInfoTypeDef;
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

#endif /* __STM32_CUSTOM_TYPE_H */
