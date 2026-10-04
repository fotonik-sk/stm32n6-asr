/**
  ******************************************************************************
  * @file    stm32_gd25lx512me.h
  * @author  MCD Application Team
  * @brief   This file includes a driver for custom support of
  *          GigaDevice GD25LX512ME Nor Flash memory
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
#ifndef __STM32_GD25LX512ME__H__
#define __STM32_GD25LX512ME__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"
#if defined(EXTMEM_DRIVER_CUSTOM) && (EXTMEM_DRIVER_CUSTOM == 1)
#include "stm32_custom_driver_api.h"
#include "stm32_custom_driver_type.h"

/** @defgroup CUSTOM CUSTOM driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Private Macro ------------------------------------------------------------*/
/** @defgroup CUSTOM_Private_Macro Private Macro
  * @{
  */
#define EXTMEM_GD25LX512ME_CMD_READ                 0x03    /*!< Command to read data */
#define EXTMEM_GD25LX512ME_CMD_WRITE                0x02    /*!< Command to write data */
#define EXTMEM_GD25LX512ME_DUMMY_READ               0x08    /*!< Number of dummy cycles for read operations */
#define EXTMEM_GD25LX512ME_DUMMY_WRITE              0x00    /*!< Number of dummy cycles for write operations */
#define EXTMEM_GD25LX512ME_DUMMY_REG_READ           8       /*!< Number of dummy cycles to read register */

#define EXTMEM_GD25LX512ME_PAGE_SIZE                256     /*!< Page size 256 Bytes */
#define EXTMEM_GD25LX512ME_SECTOR_SIZE              0x1000  /*!< Sector size in bytes (4K) */
#define EXTMEM_GD25LX512ME_CMD_ERASE_SECTOR         0x20    /*!< Command to erase a sector */
#define EXTMEM_GD25LX512ME_CMD_MASS_ERASE           0x60    /*!< Command to perform mass erase */
#define EXTMEM_GD25LX512ME_CMD_RDSR                 0x05    /*!< Command to read status register */
#define EXTMEM_GD25LX512ME_CMD_WE                   0x06    /*!< Command to enable write operations */
#define EXTMEM_GD25LX512ME_MASK_WEL                 0x02    /*!< Mask for write enable latch */
#define EXTMEM_GD25LX512ME_VALUE_WEL                0x02    /*!< Match Value for write enable latch */
#define EXTMEM_GD25LX512ME_MASK_WIP                 0x01    /*!< Mask for write in progress */
#define EXTMEM_GD25LX512ME_VALUE_WIP                0x00    /*!< Match Value for write in progress */

/* 1st configuration step : Read and Write Volatile Configuration Register (Addr 3) - Driver Strength configuration 25 Ohm (0xFD). Depending on board */
#define EXTMEM_GD25LX512ME_CMD1_READ_VCR            0x85    /*!< Command to read from a Volatile Configuration register */
#define EXTMEM_GD25LX512ME_CMD1_WRITE_VCR           0x81    /*!< Command to write to a volatile configuration register */
#define EXTMEM_GD25LX512ME_CMD1_ADDR_VCR            0x03    /*!< Address of the register */
#define EXTMEM_GD25LX512ME_CMD1_VALUE_VCR           0xFD    /*!< Value of the register */
#define EXTMEM_GD25LX512ME_CMD1_MASK_VCR            0xFF    /*!< Mask of the register */

/* 1st configuration step : Read and Write Volatile Configuration Register (Addr 0) - I/O mode to Octal DTR with DQS (0xE7) */
#define EXTMEM_GD25LX512ME_CMD2_READ_VCR            0x85    /*!< Command to read from a Volatile Configuration register */
#define EXTMEM_GD25LX512ME_CMD2_WRITE_VCR           0x81    /*!< Command to write to a volatile configuration register */
#define EXTMEM_GD25LX512ME_CMD2_ADDR_VCR            0x00    /*!< Address of the register */
#define EXTMEM_GD25LX512ME_CMD2_VALUE_VCR           0xE7    /*!< Value of the register */
#define EXTMEM_GD25LX512ME_CMD2_MASK_VCR            0xFF    /*!< Mask of the register */

#define EXTMEM_GD25LX512ME_OCTAL_CMD_ERASE_SECTOR   0x20    /*!< Command to erase a sector */
#define EXTMEM_GD25LX512ME_OCTAL_CMD_MASS_ERASE     0x60    /*!< Command to perform mass erase */
#define EXTMEM_GD25LX512ME_OCTAL_CMD_READ           0xFD    /*!< Command to read data */
#define EXTMEM_GD25LX512ME_OCTAL_CMD_WRITE          0x02    /*!< Command to write data */
#define EXTMEM_GD25LX512ME_OCTAL_DUMMY_READ         16      /*!< Number of dummy cycles for read operations */
#define EXTMEM_GD25LX512ME_OCTAL_DUMMY_WRITE        0       /*!< Number of dummy cycles for write operations */
#define EXTMEM_GD25LX512ME_OCTAL_DUMMY_REG_READ     8       /*!< Number of dummy cycles to read register */
#define EXTMEM_GD25LX512ME_OCTAL_CMD_RDSR           0x05    /*!< Command to read status register in octal mode */
#define EXTMEM_GD25LX512ME_OCTAL_CMD_WE             0x06    /*!< Command to enable write operations in octal mode */
#define EXTMEM_GD25LX512ME_OCTAL_MASK_WEL           0x02    /*!< Mask for write enable latch */
#define EXTMEM_GD25LX512ME_OCTAL_VALUE_WEL          0x02    /*!< Match Value for write enable latch */
#define EXTMEM_GD25LX512ME_OCTAL_MASK_WIP           0x01    /*!< Mask for write in progress */
#define EXTMEM_GD25LX512ME_OCTAL_VALUE_WIP          0x00    /*!< Match Value for write in progress */
/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

#define EXTMEM_GD25LX512ME_INIT { \
                                   .MemType                                  = EXTMEM_CUSTOM_NOR_FLASH,               /*!< Common field for memory type        */ \
                                   .MemStandard                              = EXTMEM_CUSTOM_STD_MICRON,              /*!< e.g., Macronix, Micron, etc.        */  \
                                   .MemSize                                  = EXTMEM_CUSTOM_SIZE_512MB,              /*!< Common field for memory Size        */ \
                                   .ResetMethod                              = EXTMEM_CUSTOM_RESET_METHOD_6666_9999,  /*!< Reset method based on 66h and 99 instructions */ \
                                   .ResetDelay                               = 100,                                   /*!< Time needed after Reset (in ms, 0 if no required delay */ \
                                   .MemChipSelectHighTimeCycle               = 10,                                    /*!< Common field for memory Chip select high time */ \
                                   .StartupConfig.Frequency                  = 50000000,                              /*!< Operating frequency in Hz*/ \
                                   .StartupConfig.CommandRead                = EXTMEM_GD25LX512ME_CMD_READ,           /*!< Command to read data */ \
                                   .StartupConfig.CommandWrite               = EXTMEM_GD25LX512ME_CMD_WRITE,          /*!< Command to write data */ \
                                   .StartupConfig.DummyCycleRead             = EXTMEM_GD25LX512ME_DUMMY_READ,         /*!< Number of dummy cycles for read operations */ \
                                   .StartupConfig.DummyCycleWrite            = EXTMEM_GD25LX512ME_DUMMY_WRITE,        /*!< Number of dummy cycles for write operations */ \
                                   .StartupConfig.DummyRegisterRead          = EXTMEM_GD25LX512ME_DUMMY_REG_READ,     /*!< Number of dummy cycles to read register */ \
                                   .StartupConfig.AccessMode                 = EXTMEM_CUSTOM_1S_1S_1S,                /*!< Access mode (e.g., 1S4D4D) */ \
                                   .StartupConfig.InstructionSize            = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,      /*!< Size of the instruction */ \
                                   .StartupConfig.AddressSize                = EXTMEM_CUSTOM_ADDRESS_24_BITS,         /*!< Size of the address */ \
                                   .StartupConfig.DqsMode                    = EXTMEM_CUSTOM_DQS_DISABLE,             /*!< Data strobe mode */ \
                                   .NbRegisterConfig                         = 3,                                     /*!< Common field for memory Chip select */ \
                                   .RegisterConfig[0].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RW_REG_ADDR,   /*!< Config step to read/write/read a register */ \
                                   .RegisterConfig[0].RWConfigStep.CommandRegisterRead    = EXTMEM_GD25LX512ME_CMD1_READ_VCR,    /*!< Command to read from a register */ \
                                   .RegisterConfig[0].RWConfigStep.CommandRegisterWrite   = EXTMEM_GD25LX512ME_CMD1_WRITE_VCR,   /*!< Command to read from a register */ \
                                   .RegisterConfig[0].RWConfigStep.RegisterAddress        = EXTMEM_GD25LX512ME_CMD1_ADDR_VCR,    /*!< Address of the register */ \
                                   .RegisterConfig[0].RWConfigStep.RegisterValue          = EXTMEM_GD25LX512ME_CMD1_VALUE_VCR,   /*!< Value of the register */ \
                                   .RegisterConfig[0].RWConfigStep.RegisterMask           = EXTMEM_GD25LX512ME_CMD1_MASK_VCR,    /*!< Mask for the register */ \
                                   .RegisterConfig[1].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RW_REG_ADDR,   /*!< Config step to read/write/read a register */ \
                                   .RegisterConfig[1].RWConfigStep.CommandRegisterRead    = EXTMEM_GD25LX512ME_CMD2_READ_VCR,    /*!< Command to read from a register */ \
                                   .RegisterConfig[1].RWConfigStep.CommandRegisterWrite   = EXTMEM_GD25LX512ME_CMD2_WRITE_VCR,   /*!< Command to read from a register */ \
                                   .RegisterConfig[1].RWConfigStep.RegisterAddress        = EXTMEM_GD25LX512ME_CMD2_ADDR_VCR,    /*!< Address of the register */ \
                                   .RegisterConfig[1].RWConfigStep.RegisterValue          = EXTMEM_GD25LX512ME_CMD2_VALUE_VCR,   /*!< Value of the register */ \
                                   .RegisterConfig[1].RWConfigStep.RegisterMask           = EXTMEM_GD25LX512ME_CMD2_MASK_VCR,    /*!< Mask for the register */ \
                                   .RegisterConfig[2].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG,  /*!< Config step to use new Optional configuration */ \
                                   .OptionalConfig.Frequency                 = 200000000,                             /*!< Operating frequency in Hz*/ \
                                   .OptionalConfig.CommandRead               = EXTMEM_GD25LX512ME_OCTAL_CMD_READ,     /*!< Command to read data */ \
                                   .OptionalConfig.CommandWrite              = EXTMEM_GD25LX512ME_OCTAL_CMD_WRITE,    /*!< Command to write data */ \
                                   .OptionalConfig.DummyCycleRead            = EXTMEM_GD25LX512ME_OCTAL_DUMMY_READ,   /*!< Number of dummy cycles for read operations */ \
                                   .OptionalConfig.DummyCycleWrite           = EXTMEM_GD25LX512ME_OCTAL_DUMMY_WRITE,  /*!< Number of dummy cycles for write operations */ \
                                   .OptionalConfig.DummyRegisterRead         = EXTMEM_GD25LX512ME_OCTAL_DUMMY_REG_READ,/*!< Number of dummy cycles to read register */ \
                                   .OptionalConfig.AccessMode                = EXTMEM_CUSTOM_8S_8D_8D,                /*!< Access mode (e.g., 1S4D4D) */ \
                                   .OptionalConfig.InstructionSize           = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,      /*!< Size of the instruction */ \
                                   .OptionalConfig.AddressSize               = EXTMEM_CUSTOM_ADDRESS_32_BITS,         /*!< Size of the address */ \
                                   .OptionalConfig.DqsMode                   = EXTMEM_CUSTOM_DQS_ENABLE,              /*!< Data strobe mode */ \
                                   .NorFlashConfig.PageSize                  = EXTMEM_GD25LX512ME_PAGE_SIZE,          /*!< Page Size */ \
                                   .NorFlashConfig.SectorSize                = EXTMEM_GD25LX512ME_SECTOR_SIZE,        /*!< Sector Size */ \
                                   .NorFlashConfig.MaxSectorEraseTime        = 300,                                   /*!< Max time expected for completing a Sector Erase operation (in ms) */ \
                                   .NorFlashConfig.MaxChipEraseTime          = 300000,                                /*!< Max time expected for completing a Chip Erase operation (in ms) */ \
                                   .NorFlashConfig.Startup.Cmd_EraseSector   = EXTMEM_GD25LX512ME_CMD_ERASE_SECTOR,   /*!< Command to erase a sector */ \
                                   .NorFlashConfig.Startup.Cmd_MassErase     = EXTMEM_GD25LX512ME_CMD_MASS_ERASE,     /*!< Command to perform mass erase */ \
                                   .NorFlashConfig.Startup.Cmd_RDSR          = EXTMEM_GD25LX512ME_CMD_RDSR,           /*!< Command to read status register */ \
                                   .NorFlashConfig.Startup.Cmd_WE            = EXTMEM_GD25LX512ME_CMD_WE,             /*!< Command to enable write operations */ \
                                   .NorFlashConfig.Startup.MatchMask_WEL     = EXTMEM_GD25LX512ME_MASK_WEL,           /*!< Mask for write enable latch */ \
                                   .NorFlashConfig.Startup.MatchValue_WEL    = EXTMEM_GD25LX512ME_VALUE_WEL,          /*!< Match Value for write enable latch */ \
                                   .NorFlashConfig.Startup.MatchMask_WIP     = EXTMEM_GD25LX512ME_MASK_WIP,           /*!< Mask for write in progress */ \
                                   .NorFlashConfig.Startup.MatchValue_WIP    = EXTMEM_GD25LX512ME_VALUE_WIP,          /*!< Match Value for write in progress */ \
                                   .NorFlashConfig.OptionalConfigEnable      = 1,                                     /*!< Optional config for NorFlash is used */ \
                                   .NorFlashConfig.Optional.Cmd_EraseSector  = EXTMEM_GD25LX512ME_OCTAL_CMD_ERASE_SECTOR,  /*!< Command to erase a sector */ \
                                   .NorFlashConfig.Optional.Cmd_MassErase    = EXTMEM_GD25LX512ME_OCTAL_CMD_MASS_ERASE,    /*!< Command to perform mass erase */ \
                                   .NorFlashConfig.Optional.Cmd_RDSR         = EXTMEM_GD25LX512ME_OCTAL_CMD_RDSR,     /*!< Command to read status register in octal mode */ \
                                   .NorFlashConfig.Optional.Cmd_WE           = EXTMEM_GD25LX512ME_OCTAL_CMD_WE,       /*!< Command to enable write operations in octal mode */ \
                                   .NorFlashConfig.Optional.MatchMask_WEL    = EXTMEM_GD25LX512ME_OCTAL_MASK_WEL,     /*!< Mask for write enable latch */ \
                                   .NorFlashConfig.Optional.MatchValue_WEL   = EXTMEM_GD25LX512ME_OCTAL_VALUE_WEL,    /*!< Match Value for write enable latch */ \
                                   .NorFlashConfig.Optional.MatchMask_WIP    = EXTMEM_GD25LX512ME_OCTAL_MASK_WIP,     /*!< Mask for write in progress */ \
                                   .NorFlashConfig.Optional.MatchValue_WIP   = EXTMEM_GD25LX512ME_OCTAL_VALUE_WIP,    /*!< Match Value for write in progress */ \
                                   }

#if defined(EXTMEM_C)
EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_gd25lx512me = EXTMEM_GD25LX512ME_INIT;
#else
extern EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_gd25lx512me;
#endif /* EXTMEM_C */

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

#endif /* __STM32_GD25LX512ME__H__ */
