/**
  ******************************************************************************
  * @file    stm32_sdcard_driver.c
  * @author  MCD Application Team
  * @brief   This file implements the SDCARD driver.
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
#if EXTMEM_DRIVER_SDCARD == 1
#if EXTMEM_SAL_SD == 1
#include "../sal/stm32_sal_sd_api.h"
#endif /* EXTMEM_SAL_SD */
#if EXTMEM_SAL_MMC == 1
#include "../sal/stm32_sal_mmc_api.h"
#endif /* EXTMEM_SAL_MMC */

#include "stm32_sdcard_driver_api.h"
#if EXTMEM_DRIVER_SDCARD_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG)
#include "string.h"
#include "stdio.h"
#endif /* EXTMEM_DRIVER_SDCARD_DEBUG_LEVEL > 0 || defined(EXTMEM_MACRO_DEBUG) */

/** @defgroup SDCARD SDCARD driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Private Macros ------------------------------------------------------------*/
/** @defgroup SDCARD_Private_Macro Private Macro
  * @{
  */

/**
  * @brief Default Timeout value (ms)
  */
#define DRIVER_DEFAULT_TIMEOUT 300

/**
  * @brief Debug macro for a string.
  */
#if EXTMEM_DRIVER_SDCARD_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG)
#define SDCARD_DEBUG_STR(_STR_)  {                      \
                                   EXTMEM_MACRO_DEBUG("\tSDCARD::");  \
                                   EXTMEM_MACRO_DEBUG(_STR_);        \
                                   EXTMEM_MACRO_DEBUG("\n\r");       \
                                   }
#else
#define SDCARD_DEBUG_STR(_STR_)
#endif /* EXTMEM_DRIVER_SDCARD_DEBUG_LEVEL > 0 || defined(EXTMEM_MACRO_DEBUG) */

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup SDCARD_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief Initializes the SDCARD driver.
  * @param Peripheral Pointer to the peripheral object.
  * @param SDCARDObject Pointer to the SDCARD driver object.
  * @retval @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
  */
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_Init(void *Peripheral,
                                                             EXTMEM_DRIVER_SDCARD_ObjectTypeDef *SDCARDObject)
{
  EXTMEM_DRIVER_SDCARD_StatusTypeDef retr = EXTMEM_DRIVER_SDCARD_OK;

  /* initialize the instance */
  SDCARD_DEBUG_STR("initialize the instance")
  switch (SDCARDObject->sdcard_public.Link)
  {
    case EXTMEM_DRIVER_SDCARD_LINKSD :
      if (HAL_OK != SAL_SD_Init(&SDCARDObject->sdcard_private.SALObject.SDObject, Peripheral,
                                &SDCARDObject->sdcard_private.Info))
      {
        retr = EXTMEM_DRIVER_SDCARD_ERROR;
      }
      break;
#if EXTMEM_SAL_MMC == 1
    case EXTMEM_DRIVER_SDCARD_LINKMMC:
      if (HAL_OK != SAL_MMC_Init(&SDCARDObject->SALObject.MMCObject, Peripheral))
      {
        retr = EXTMEM_DRIVER_SDCARD_ERROR;
      }
      break;
#endif /* EXTMEM_SAL_MMC == 1 */
    default :
      retr = EXTMEM_DRIVER_SDCARD_ERROR_LINKTYPE;
      break;
  }

  return retr;
}

/**
  * @brief Deinitializes the SDCARD driver.
  * @param SDCARDObject Pointer to the SDCARD driver object.
  * @retval @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
  */
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_DeInit(EXTMEM_DRIVER_SDCARD_ObjectTypeDef *SDCARDObject)
{
  EXTMEM_DRIVER_SDCARD_StatusTypeDef retr = EXTMEM_DRIVER_SDCARD_OK;
  SDCARDObject->sdcard_private.Info.BlockNbr = 0;
  return retr;
}

/**
  * @brief Reads data from the SDCARD memory.
  * @param SDCARDObject Pointer to the SDCARD driver object.
  * @param Address Memory address.
  * @param Data Pointer to the data buffer to store the read data.
  * @param Size Size of data to read (in bytes).
  * @retval @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
  */
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_Read(EXTMEM_DRIVER_SDCARD_ObjectTypeDef *SDCARDObject,
                                                             uint32_t Address, uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_SDCARD_StatusTypeDef retr = EXTMEM_DRIVER_SDCARD_OK;
  HAL_StatusTypeDef hal_status = HAL_OK;
  uint32_t block_size = SDCARDObject->sdcard_private.Info.BlockSize;

  if (block_size == 0u)
  {
    retr = EXTMEM_DRIVER_SDCARD_ERROR;
  }
  else
  {
    uint32_t block_num = Address / block_size;
    uint32_t block_count = Size / block_size;

    /* Check if Address and size are multiple of block size */
    if (((Address % block_size) != 0u) || ((Size % block_size) != 0u))
    {
      retr = EXTMEM_DRIVER_SDCARD_ERROR_PARAM;
      goto error;
    }

    if (block_count > SDCARDObject->sdcard_private.Info.BlockNbr)
    {
      retr = EXTMEM_DRIVER_SDCARD_ERROR_PARAM;
      goto error;
    }

    switch (SDCARDObject->sdcard_public.Link)
    {
      case EXTMEM_DRIVER_SDCARD_LINKSD :
        hal_status = SAL_SD_ReadData(&SDCARDObject->sdcard_private.SALObject.SDObject, block_num, Data, block_count);
        break;
#if EXTMEM_SAL_MMC == 1
      case EXTMEM_DRIVER_SDCARD_LINKMMC:
        hal_status = SAL_MMC_Read(&SDCARDObject->sdcard_private.SALObject.MMCObject, block_num, Data, block_count);
        break;
#endif /* EXTMEM_SAL_MMC == 1 */
      default :
        retr = EXTMEM_DRIVER_SDCARD_ERROR_LINKTYPE;
        break;
    }

    if (HAL_OK != hal_status)
    {
      retr = EXTMEM_DRIVER_SDCARD_ERROR_READ;
    }
  }
error:
  return retr;
}

/**
  * @brief Writes data to the SDCARD memory.
  * @param SDCARDObject Pointer to the SDCARD driver object.
  * @param Address Memory address.
  * @param Data Pointer to the data buffer to be written.
  * @param Size Size of data to be written (in bytes).
  * @retval @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
  */
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_Write(EXTMEM_DRIVER_SDCARD_ObjectTypeDef *SDCARDObject,
                                                              uint32_t Address, const uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_SDCARD_StatusTypeDef retr = EXTMEM_DRIVER_SDCARD_OK;
  HAL_StatusTypeDef hal_status = HAL_OK;
  uint32_t block_size = SDCARDObject->sdcard_private.Info.BlockSize;
  if (block_size == 0u)
  {
    retr = EXTMEM_DRIVER_SDCARD_ERROR;
  }
  else
  {
    uint32_t block_num = Address / block_size;
    uint32_t block_count = Size / block_size;

    /* Check if Address and size are multiple of block size */
    if (((Address % block_size) != 0u) || ((Size % block_size) != 0u))
    {
      retr = EXTMEM_DRIVER_SDCARD_ERROR_PARAM;
      goto error;
    }

    if (block_count > SDCARDObject->sdcard_private.Info.BlockNbr)
    {
      retr = EXTMEM_DRIVER_SDCARD_ERROR_PARAM;
      goto error;
    }

    switch (SDCARDObject->sdcard_public.Link)
    {
      case EXTMEM_DRIVER_SDCARD_LINKSD :
        hal_status = SAL_SD_WriteData(&SDCARDObject->sdcard_private.SALObject.SDObject, block_num, Data, block_count);
        break;
      case EXTMEM_DRIVER_SDCARD_LINKMMC:
      default :
        retr = EXTMEM_DRIVER_SDCARD_ERROR_LINKTYPE;
        break;
    }

    if (HAL_OK != hal_status)
    {
      retr = EXTMEM_DRIVER_SDCARD_ERROR_WRITE;
    }
  }
error:
  return retr;
}

/**
  * @brief Erases blocks in the SDCARD memory.
  * @param SDCARDObject Pointer to the SDCARD driver object.
  * @param Address Memory address.
  * @param Size Size of data to erase (in bytes).
  * @retval @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
  */
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_EraseBlock(EXTMEM_DRIVER_SDCARD_ObjectTypeDef *SDCARDObject,
                                                                   uint32_t Address, uint32_t Size)
{
  EXTMEM_DRIVER_SDCARD_StatusTypeDef retr = EXTMEM_DRIVER_SDCARD_OK;
  HAL_StatusTypeDef hal_status = HAL_OK;
  uint32_t block_size = SDCARDObject->sdcard_private.Info.BlockSize;

  if (block_size == 0u)
  {
    retr = EXTMEM_DRIVER_SDCARD_ERROR;
  }
  else
  {
    uint32_t block_num = Address / block_size;
    uint32_t block_count = Size / block_size;

    if ((Size % block_size) != 0u)
    {
      block_count = block_count + 1u;
    }

    /* Check if Address is multiple of block size */
    if ((Address % block_size) != 0u)
    {
      retr = EXTMEM_DRIVER_SDCARD_ERROR_PARAM;
      goto error;
    }

    if (block_count > SDCARDObject->sdcard_private.Info.BlockNbr)
    {
      retr = EXTMEM_DRIVER_SDCARD_ERROR_PARAM;
      goto error;
    }

    switch (SDCARDObject->sdcard_public.Link)
    {
      case EXTMEM_DRIVER_SDCARD_LINKSD :
        hal_status = SAL_SD_EraseBlock(&SDCARDObject->sdcard_private.SALObject.SDObject, block_num, block_count);
        break;
      case EXTMEM_DRIVER_SDCARD_LINKMMC:
      default :
        retr = EXTMEM_DRIVER_SDCARD_ERROR_LINKTYPE;
        break;
    }

    if (HAL_OK != hal_status)
    {
      retr = EXTMEM_DRIVER_SDCARD_ERROR;
    }
  }
error:
  return retr;
}

/**
  * @brief Performs a mass erase of the SDCARD memory.
  * @param SDCARDObject Pointer to the SDCARD driver object.
  * @retval @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
  */
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_Erase(EXTMEM_DRIVER_SDCARD_ObjectTypeDef *SDCARDObject)
{
  EXTMEM_DRIVER_SDCARD_StatusTypeDef retr = EXTMEM_DRIVER_SDCARD_OK;
  HAL_StatusTypeDef hal_status = HAL_OK;
  switch (SDCARDObject->sdcard_public.Link)
  {
    case EXTMEM_DRIVER_SDCARD_LINKSD :
      hal_status = SAL_SD_MassErase(&SDCARDObject->sdcard_private.SALObject.SDObject);
      break;
    case EXTMEM_DRIVER_SDCARD_LINKMMC:
    default :
      retr = EXTMEM_DRIVER_SDCARD_ERROR_LINKTYPE;
      break;
  }

  if (HAL_OK != hal_status)
  {
    retr = EXTMEM_DRIVER_SDCARD_ERROR;
  }
  return retr;
}

/**
  * @brief Retrieves SDCARD information.
  * @param SDCARDObject Pointer to the SDCARD driver object.
  * @param Info Pointer to the SDCARD information structure to be filled.
  * @retval @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
  */
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_GetInfo(EXTMEM_DRIVER_SDCARD_ObjectTypeDef *SDCARDObject,
                                                                EXTMEM_DRIVER_SDCARD_InfoTypeDef *Info)
{
  Info->BlockSize = SDCARDObject->sdcard_private.Info.BlockSize;
  Info->BlockNbr  = SDCARDObject->sdcard_private.Info.BlockNbr;
  return EXTMEM_DRIVER_SDCARD_OK;
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
