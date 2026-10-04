/**
 * @file     drv_pdm_mdf.c
 * @brief    MDF1 filter 0 configuration for a PDM microphone on CCK0/SDI0.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "drv_pdm_mdf.h"

#include "drv_pdm_dma.h"
#include "mcu_clock.h"

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief HAL handle of MDF1 filter 0. */
static MDF_HandleTypeDef mdf;

/** @brief Filter settings replayed on every acquisition start. */
static MDF_FilterConfigTypeDef filter;

/** @brief Set when a clock, GPIO or DMA attachment step failed. */
static volatile uint8_t error;

/** @brief Board hook that routes CCK0/SDI0, called from HAL_MDF_MspInit(). */
static void (*board_init_pins)(void);

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Configure the MDF1 common parameters and the microphone serial interface.
 * @return HAL_OK on success, HAL_ERROR when HAL_MDF_Init() fails.
 * @note The microphone clock on CCK0 is the 49.14 MHz IC8 kernel clock / 2
 *       (processing clock) / 12 = about 2.048 MHz. The bitstream is sampled on the
 *       falling edge.
 */
static HAL_StatusTypeDef init_filter(void)
{
    mdf.Instance = MDF1_Filter0;
    mdf.Init.CommonParam.InterleavedFilters = 0U;
    mdf.Init.CommonParam.ProcClockDivider = 2U;
    mdf.Init.CommonParam.OutputClock.Activation = ENABLE;
    mdf.Init.CommonParam.OutputClock.Pins = MDF_OUTPUT_CLOCK_0;
    mdf.Init.CommonParam.OutputClock.Divider = 12U;
    mdf.Init.CommonParam.OutputClock.Trigger.Activation = DISABLE;
    mdf.Init.SerialInterface.Activation = ENABLE;
    mdf.Init.SerialInterface.Mode = MDF_SITF_NORMAL_SPI_MODE;
    mdf.Init.SerialInterface.ClockSource = MDF_SITF_CCK0_SOURCE;
    mdf.Init.SerialInterface.Threshold = 31U;
    mdf.Init.FilterBistream = MDF_BITSTREAM0_FALLING;
    return HAL_MDF_Init(&mdf);
}

/**
 * @brief Fill the digital filter chain applied to the microphone bitstream.
 * @note SINC4 decimating by 32, a reshape filter decimating by 4 and a high-pass
 *       filter at 0.000625 x Fpcm. 32 x 4 = 128 turns the 2.048 MHz bitstream
 *       into about 16 kHz PCM. Changing any of these changes the PCM characteristics
 *       the ASR front-end expects.
 */
static void init_filter_config(void)
{
    filter.DataSource = MDF_DATA_SOURCE_BSMX;
    filter.Delay = 0U;
    filter.CicMode = MDF_ONE_FILTER_SINC4;
    filter.DecimationRatio = 32U;
    filter.Offset = 0;
    filter.Gain = 16U;
    filter.ReshapeFilter.Activation = ENABLE;
    filter.ReshapeFilter.DecimationRatio = MDF_RSF_DECIMATION_RATIO_4;
    filter.HighPassFilter.Activation = ENABLE;
    filter.HighPassFilter.CutOffFrequency = MDF_HPF_CUTOFF_0_000625FPCM;
    filter.Integrator.Activation = DISABLE;
    filter.SoundActivity.Activation = DISABLE;
    filter.AcquisitionMode = MDF_MODE_ASYNC_CONT;
    filter.FifoThreshold = MDF_FIFO_THRESHOLD_NOT_EMPTY;
    filter.DiscardSamples = 0U;
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

HAL_StatusTypeDef drv_pdm_mdf_init(void (*init_pins)(void))
{
    error = 0U;
    board_init_pins = init_pins;

    /* PLL3 feeds IC8, the MDF1 kernel clock; the FSBL leaves it bypassed. */
    if (!mcu_clock_enable_audio_pll())
    {
        error = 1U;
        return HAL_ERROR;
    }
    if ((init_filter() != HAL_OK) || (error != 0U))
    {
        error = 1U;
        return HAL_ERROR;
    }

    init_filter_config();
    return HAL_OK;
}

HAL_StatusTypeDef drv_pdm_mdf_start(uint32_t address, uint32_t data_length)
{
    MDF_DmaConfigTypeDef dma_config = {
        .Address = address,
        .DataLength = data_length,
        .MsbOnly = ENABLE,
    };

    return HAL_MDF_AcqStart_DMA(&mdf, &filter, &dma_config);
}

HAL_StatusTypeDef drv_pdm_mdf_stop(void)
{
    return HAL_MDF_AcqStop_DMA(&mdf);
}

/**
 * @brief Enable the MDF1 kernel clock, route the microphone pins and link the
 *        capture queue.
 * @param[in,out] hmdf MDF handle being initialized by HAL_MDF_Init().
 * @note Called by HAL. The MDF1 kernel clock is taken from IC8, sourced by PLL3.
 */
void HAL_MDF_MspInit(MDF_HandleTypeDef *hmdf)
{
    RCC_PeriphCLKInitTypeDef peripheral_clock = {0};

    if (!IS_MDF_INSTANCE(hmdf->Instance))
    {
        return;
    }

    peripheral_clock.PeriphClockSelection = RCC_PERIPHCLK_MDF1;
    peripheral_clock.Mdf1ClockSelection = RCC_MDF1CLKSOURCE_IC8;
    peripheral_clock.ICSelection[RCC_IC8].ClockSelection = RCC_ICCLKSOURCE_PLL3;
    peripheral_clock.ICSelection[RCC_IC8].ClockDivider = 1U;
    if (HAL_RCCEx_PeriphCLKConfig(&peripheral_clock) != HAL_OK)
    {
        error = 1U;
        return;
    }

    __HAL_RCC_MDF1_CLK_ENABLE();
    if (board_init_pins == NULL)
    {
        error = 1U;
        return;
    }
    board_init_pins();

    if (drv_pdm_dma_link(hmdf) != HAL_OK)
    {
        error = 1U;
    }
}

/**
 * @brief Release the MDF1 clock and the capture queue.
 * @param[in,out] hmdf MDF handle being de-initialized by HAL_MDF_DeInit().
 * @note Called by HAL. The pins stay routed; they belong to the board.
 */
void HAL_MDF_MspDeInit(MDF_HandleTypeDef *hmdf)
{
    if (IS_MDF_INSTANCE(hmdf->Instance))
    {
        __HAL_RCC_MDF1_CLK_DISABLE();
        drv_pdm_dma_unlink(hmdf);
    }
}
