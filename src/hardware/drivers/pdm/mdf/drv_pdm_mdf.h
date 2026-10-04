/**
 * @file     drv_pdm_mdf.h
 * @brief    MDF1 filter 0 configuration for a PDM microphone on CCK0/SDI0.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_DRIVERS_PDM_MDF_DRV_PDM_MDF_H
#define HARDWARE_DRIVERS_PDM_MDF_DRV_PDM_MDF_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "stm32n6xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/**
 * @brief Nominal PCM sample rate produced by this configuration.
 * @note PLL3 runs at 49.142 MHz, the microphone clock is divided by 12 and the
 *       filter decimates by 32 then by 4.
 */
#define DRV_PDM_MDF_SAMPLE_RATE_HZ 16000U

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Configure PLL3, MDF1 filter 0 and the microphone serial interface.
 * @param[in] init_pins Board hook that routes CCK0/SDI0; called from the MSP
 *            hook once MDF1 is clocked.
 * @return HAL_OK on success, HAL_ERROR when a clock or HAL step fails.
 * @pre drv_pdm_dma_configure() succeeded: HAL_MDF_Init() calls the MSP hook, which
 *      builds the capture queue through drv_pdm_dma_link().
 * @note Acquisition is not started here. The configuration is fixed and matches
 *       the settings the ASR front-end was calibrated against.
 */
HAL_StatusTypeDef drv_pdm_mdf_init(void (*init_pins)(void));

/**
 * @brief Start continuous acquisition into the DMA capture area.
 * @param[in] address First byte of the capture area.
 * @param[in] data_length Bytes transferred by the head linked-list node.
 * @return HAL_OK on success, HAL_ERROR when the acquisition cannot start.
 * @pre drv_pdm_mdf_init() succeeded and the DMA queue is linked.
 * @note Only the most significant half-word of each filter sample is stored,
 *       which yields PCM16 samples.
 */
HAL_StatusTypeDef drv_pdm_mdf_start(uint32_t address, uint32_t data_length);

/**
 * @brief Stop acquisition and the associated DMA transfer.
 * @return HAL_OK on success, HAL_ERROR when the HAL stop sequence fails.
 */
HAL_StatusTypeDef drv_pdm_mdf_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_DRIVERS_PDM_MDF_DRV_PDM_MDF_H */
