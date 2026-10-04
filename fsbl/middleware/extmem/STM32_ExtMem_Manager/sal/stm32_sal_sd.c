/**
  ******************************************************************************
  * @file   stm32_sal_sd.c
  * @author  MCD Application Team
  * @brief   This file implements the software adaptation layer for SD.
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
#if EXTMEM_SAL_SD_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG)
#include <stdio.h>
#endif /* EXTMEM_SAL_SD_DEBUG_LEVEL != 0 && EXTMEM_MACRO_DEBUG */

#if EXTMEM_SAL_SD == 1
#include "stm32_sal_sd_type.h"
#include "stm32_sal_sd_api.h"

/** @defgroup SAL_SD SAL_SD : Software adaptation layer for the HAL_SD
  * @ingroup  EXTMEM_SAL
  * @{
  */

/**
  * @brief Ready timeout value
  */
#define READY_TIMEOUT 1000u

/* Private Macros ------------------------------------------------------------*/

/** @defgroup SAL_SD_Private_Macros SAL SD Private Macros
  * @{
  */
#if EXTMEM_SAL_SD_DEBUG_LEVEL == 0 || !defined(EXTMEM_MACRO_DEBUG)
#define DEBUG_PARAM_BEGIN()
#define DEBUG_PARAM_DATA(_STR_)
#define DEBUG_PARAM_INT(_INT_ )
#define DEBUG_PARAM_END()
#else
/**
  * @brief Trace header macro.
  */
#define DEBUG_PARAM_BEGIN()     EXTMEM_MACRO_DEBUG("\t\tSALSD::");

/**
  * @brief Trace data string macro.
  */
#define DEBUG_PARAM_DATA(_STR_) EXTMEM_MACRO_DEBUG(_STR_);

/**
  * @brief Trace data integer macro
  */
#define DEBUG_PARAM_INT(_INT_ ) {                                                 \
                                  char str[10];                                   \
                                  (void)snprintf(str, sizeof(str), "0x%x", _INT_);\
                                  EXTMEM_MACRO_DEBUG(str);                        \
                                  }
/**
  * @brief Trace close macro
  */
#define DEBUG_PARAM_END()       EXTMEM_MACRO_DEBUG("\n");

#endif /* EXTMEM_SFDP_DEBUG */

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup SAL_SD_Private_Functions SAL XSP Private Functions
  * @{
  */

/**
  * @}
  */

/* Exported variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup SAL_SD_Exported_Functions SAL XSP Exported Functions
  * @{
  */

/**
  * @brief This function initializes the SD SAL context
  * @param SalSD SAL SD Object
  * @param HALHandle HAL handle
  * @param Info  Information on the SD card
  * @return @ref HAL_StatusTypeDef
  **/
HAL_StatusTypeDef SAL_SD_Init(SAL_SD_ObjectTypeDef *SalSD, void *HALHandle, EXTMEM_DRIVER_SDCARD_InfoTypeDef *Info)
{
  SalSD->hSD = (SD_HandleTypeDef *)HALHandle;

  Info->BlockNbr  = SalSD->hSD->SdCard.BlockNbr;
  Info->BlockSize = SalSD->hSD->SdCard.BlockSize;

  /* Switch to the highest Speed mode supported by the sd-card */
  return HAL_SD_ConfigSpeedBusOperation(SalSD->hSD, SDMMC_SPEED_MODE_AUTO);
}

/**
  * @brief This function un-initializes the SD SAL context
  * @param SalSD SAL SD Object
  * @return @ref HAL_StatusTypeDef
  **/
HAL_StatusTypeDef SAL_SD_DeInit(SAL_SD_ObjectTypeDef *SalSD)
{
  SalSD->hSD->SdCard.BlockNbr = 0;
  return HAL_OK;
}

/**
  * @brief This function reads data from the SD
  * @param SalSD SAL SD Object
  * @param BlockIdx Block index
  * @param Data Data pointer
  * @param NumberOfBlock Number of block
  * @return @ref HAL_StatusTypeDef
  **/
HAL_StatusTypeDef SAL_SD_ReadData(SAL_SD_ObjectTypeDef *SalSD, uint32_t BlockIdx, uint8_t *Data, uint32_t NumberOfBlock)
{
  uint32_t timeout = 100UL * NumberOfBlock;
  HAL_StatusTypeDef retr = HAL_SD_ReadBlocks(SalSD->hSD, (uint8_t *)Data, BlockIdx, NumberOfBlock, timeout);
  if (HAL_OK == retr)
  {
    timeout = HAL_GetTick();
    /* Check the SD status */
    while (HAL_SD_GetCardState(SalSD->hSD) != HAL_SD_CARD_TRANSFER)
    {
      if ((HAL_GetTick() - timeout) > READY_TIMEOUT)
      {
        retr = HAL_TIMEOUT;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief This function writes data on the SD
  * @param SalSD SAL SD Object
  * @param BlockIdx block index
  * @param Data data pointer
  * @param NumberOfBlock number of block
  * @return @ref HAL_StatusTypeDef
  **/
HAL_StatusTypeDef SAL_SD_WriteData(SAL_SD_ObjectTypeDef *SalSD, uint32_t BlockIdx, const uint8_t *const Data,
                                   uint32_t NumberOfBlock)
{
  uint32_t timeout = 100UL * NumberOfBlock;
  HAL_StatusTypeDef retr;

  retr = HAL_SD_WriteBlocks(SalSD->hSD, Data, BlockIdx, NumberOfBlock, timeout);
  if (HAL_OK == retr)
  {
    timeout = HAL_GetTick();
    /* Check the SD status */
    while (HAL_SD_GetCardState(SalSD->hSD) != HAL_SD_CARD_TRANSFER)
    {
      if ((HAL_GetTick() - timeout) > READY_TIMEOUT)
      {
        retr = HAL_TIMEOUT;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief This function erases an amount of blocks on the SD
  * @param SalSD SAL SD Object
  * @param BlockIdx block index
  * @param BlockCount number of block
  * @return @ref HAL_StatusTypeDef
  **/
HAL_StatusTypeDef SAL_SD_EraseBlock(SAL_SD_ObjectTypeDef *SalSD, uint32_t BlockIdx, uint32_t BlockCount)
{
  uint32_t timeout;
  HAL_StatusTypeDef retr;

  retr = HAL_SD_Erase(SalSD->hSD, BlockIdx, BlockIdx + BlockCount);
  if (HAL_OK == retr)
  {
    timeout = HAL_GetTick();
    /* Check the SD status */
    while (HAL_SD_GetCardState(SalSD->hSD) != HAL_SD_CARD_TRANSFER)
    {
      if ((HAL_GetTick() - timeout) > (READY_TIMEOUT * BlockCount))
      {
        retr = HAL_TIMEOUT;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief This function erases all the SD
  * @param SalSD SAL SD Object
  * @return @ref HAL_StatusTypeDef
  **/
HAL_StatusTypeDef SAL_SD_MassErase(SAL_SD_ObjectTypeDef *SalSD)
{
  return SAL_SD_EraseBlock(SalSD, 0, SalSD->hSD->SdCard.BlockNbr);
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

#endif /* EXTMEM_SAL_SD == 1 */
