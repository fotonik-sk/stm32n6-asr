/**
  ******************************************************************************
  * @file    stm32_user_driver.c
  * @author  MCD Application Team
  * @brief   This file implements the USER driver.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"
#if EXTMEM_DRIVER_USER == 1
#include "stm32_user_driver_api.h"
#include "stm32_user_driver_type.h"

/** @defgroup USER USER driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Private Macro ------------------------------------------------------------*/
/** @defgroup USER_Private_Macro Private Macro
  * @{
  */

/**
  * @}
  */
/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup USER_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief Initializes the USER driver.
  * @param MemoryId Memory ID.
  * @param UserObject Pointer to the USER driver object.
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Init(uint32_t MemoryId,
                                                                EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  UserObject->MemID = MemoryId;     /* Store the memory ID; can be used to control multiple user memories. */
  UserObject->PtrUserDriver = NULL; /* Can be used to link data with the memory ID. */
  return retr;
}

/**
  * @brief Deinitializes the USER driver.
  * @param UserObject Pointer to the USER driver object.
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_DeInit(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)UserObject;
  return retr;
}

/**
  * @brief Reads data from the USER memory.
  * @param UserObject Pointer to the USER driver object.
  * @param Address Memory address.
  * @param Data Pointer to the data buffer to store the read data.
  * @param Size Size of data to read (in bytes).
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Read(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                                uint32_t Address, uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)Address;
  (void)Data;
  (void)Size;
  return retr;
}

/**
  * @brief Writes data to the USER memory.
  * @param UserObject Pointer to the USER driver object.
  * @param Address Memory address.
  * @param Data Pointer to the data buffer to be written.
  * @param Size Size of data to be written (in bytes).
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Write(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                                 uint32_t Address, const uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)Address;
  (void)Data;
  (void)Size;
  return retr;
}

/**
  * @brief Erases sectors in the USER memory.
  * @param UserObject Pointer to the USER driver object.
  * @param Address Memory address.
  * @param Size Size of data to erase (in bytes).
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_EraseSector(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                                       uint32_t Address, uint32_t Size)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)Address;
  (void)Size;
  return retr;
}

/**
  * @brief Performs a mass erase of the USER memory.
  * @param UserObject Pointer to the USER driver object.
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_MassErase(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  return retr;
}

/**
  * @brief Enables memory-mapped mode for the USER device.
  * @param UserObject Pointer to the USER driver object.
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Enable_MemoryMappedMode(EXTMEM_DRIVER_USER_ObjectTypeDef
    *UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  return retr;
}

/**
  * @brief Disables memory-mapped mode for the USER device.
  * @param UserObject Pointer to the USER driver object.
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Disable_MemoryMappedMode(
  EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  return retr;
}

/**
  * @brief Retrieves the mapped address.
  * @param UserObject Pointer to the USER driver object.
  * @param BaseAddress Pointer to store the mapped base address.
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_GetMapAddress(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                                         uint32_t *BaseAddress)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)BaseAddress;
  return retr;
}

/**
  * @brief Retrieves USER memory information.
  * @param UserObject Pointer to the USER driver object.
  * @param MemInfo Pointer to the USER memory information structure to be filled.
  * @retval @ref EXTMEM_DRIVER_USER_StatusTypeDef
  */
__weak EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_GetInfo(EXTMEM_DRIVER_USER_ObjectTypeDef *UserObject,
                                                                   EXTMEM_USER_MemInfoTypeDef *MemInfo)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)MemInfo;
  return retr;
}

/**
  * @}
  */

/** @addtogroup USER_Private_Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_USER == 1 */
