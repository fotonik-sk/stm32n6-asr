/**
 * @file     drv_ltdc.c
 * @brief    LTDC driver implementation; implements drv_ltdc.h.
 * @details  Adapted from the legacy lcd_word_display and display_text modules.
 *           The module owns LTDC configuration and the double-buffer swap; the
 *           board supplies the panel description and the buffers.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "drv_ltdc.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "mcu_rif.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief LTDC layer used for the RGB565 display framebuffer. */
#define LTDC_LAYER_INDEX 0U

/** @brief Maximum valid divider value for an RCC intermediate clock. */
#define LTDC_IC_MAX_DIVIDER 256U

/** @brief Maximum time to wait for an LTDC vertical-blanking reload. */
#define LTDC_RELOAD_TIMEOUT_MS 50U

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Private LTDC handle. */
static LTDC_HandleTypeDef ltdc;

/** @brief Panel description accepted by drv_ltdc_init(). */
static drv_ltdc_config_t panel;

/** @brief Byte size of one framebuffer, derived from the panel geometry. */
static uint32_t framebuffer_size_bytes;

/** @brief Set only after all initialization steps succeed. */
static bool initialized;

/** @brief Framebuffer currently scanned by LTDC. */
static uint16_t *front_buffer;

/** @brief Hidden framebuffer available to the CPU. */
static uint16_t *draw_buffer;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Configure IC16 to provide the LCD pixel clock from PLL1.
 * @return HAL_OK when the selected PLL1 frequency divides exactly to the pixel
 *         clock requested by the panel description; HAL_ERROR otherwise.
 */
static HAL_StatusTypeDef ltdc_configure_clock(void)
{
    RCC_PeriphCLKInitTypeDef clock_config = {0};
    uint32_t pll_clock = HAL_RCCEx_GetPLL1CLKFreq();
    uint32_t divider;

    if (panel.pixel_clock_hz == 0U)
    {
        return HAL_ERROR;
    }
    divider = pll_clock / panel.pixel_clock_hz;
    if ((divider == 0U) || (divider > LTDC_IC_MAX_DIVIDER) ||
        ((pll_clock % panel.pixel_clock_hz) != 0U))
    {
        return HAL_ERROR;
    }

    /* The runtime PLL1 frequency is divided to the requested pixel clock. */
    clock_config.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    clock_config.LtdcClockSelection = RCC_LTDCCLKSOURCE_IC16;
    clock_config.ICSelection[RCC_IC16].ClockSelection = RCC_ICCLKSOURCE_PLL1;
    clock_config.ICSelection[RCC_IC16].ClockDivider = divider;
    return HAL_RCCEx_PeriphCLKConfig(&clock_config);
}

/**
 * @brief Clean one framebuffer from D-cache to the LTDC-visible memory.
 * @param[in] framebuffer RGB565 framebuffer with the configured byte size.
 */
static void ltdc_clean_framebuffer(const uint16_t *framebuffer)
{
#if (__DCACHE_PRESENT == 1U)
    /* The BSP places both framebuffers on cache-line boundaries. */
    SCB_CleanDCache_by_Addr((uint32_t *)(uintptr_t)framebuffer,
                           (int32_t)framebuffer_size_bytes);
#else
    (void)framebuffer;
#endif
    __DSB();
}

/**
 * @brief Initialize LTDC with the configured panel timing and RGB565 layer.
 * @return HAL_OK after LTDC, layer, burst length, and underrun threshold setup;
 *         otherwise the failing HAL status.
 */
static HAL_StatusTypeDef ltdc_configure_controller(void)
{
    LTDC_LayerCfgTypeDef layer = {0};
    HAL_StatusTypeDef status;

    ltdc.Instance = LTDC;
    ltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    ltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    ltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    ltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    ltdc.Init.HorizontalSync = panel.horizontal_sync;
    ltdc.Init.VerticalSync = panel.vertical_sync;
    ltdc.Init.AccumulatedHBP = panel.accumulated_hbp;
    ltdc.Init.AccumulatedVBP = panel.accumulated_vbp;
    ltdc.Init.AccumulatedActiveW = panel.accumulated_active_width;
    ltdc.Init.AccumulatedActiveH = panel.accumulated_active_height;
    ltdc.Init.TotalWidth = panel.total_width;
    ltdc.Init.TotalHeigh = panel.total_height;
    ltdc.Init.Backcolor.Red = 0U;
    ltdc.Init.Backcolor.Green = 0U;
    ltdc.Init.Backcolor.Blue = 0U;
    status = HAL_LTDC_Init(&ltdc);
    if (status != HAL_OK)
    {
        return status;
    }

    layer.WindowX0 = 0U;
    layer.WindowX1 = panel.width;
    layer.WindowY0 = 0U;
    layer.WindowY1 = panel.height;
    layer.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    layer.Alpha = 255U;
    layer.Alpha0 = 0U;
    layer.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer.FBStartAdress = (uint32_t)panel.framebuffer0;
    layer.ImageWidth = panel.width;
    layer.ImageHeight = panel.height;
    status = HAL_LTDC_ConfigLayer(&ltdc, &layer, LTDC_LAYER_INDEX);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_LTDC_ConfigBurstLength(&ltdc, 16U, LTDC_LAYER_INDEX);
    if (status != HAL_OK)
    {
        return status;
    }
    return HAL_LTDC_ConfigUnderrunThreshold(&ltdc, 0xFFFFU);
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Initialize the LTDC, route the RGB bus and clear both framebuffers.
 * @param[in] config Panel description supplied by the BSP.
 * @return HAL_OK on success or the first HAL failure during initialization.
 * @note Repeated calls succeed without reconfiguring an initialized display.
 */
HAL_StatusTypeDef drv_ltdc_init(const drv_ltdc_config_t *config)
{
    HAL_StatusTypeDef status;

    if (initialized)
    {
        return HAL_OK;
    }
    if ((config == NULL) || (config->init_pins == NULL) ||
        (config->width == 0U) || (config->height == 0U) ||
        (config->framebuffer0 == 0U) || (config->framebuffer1 == 0U))
    {
        return HAL_ERROR;
    }
    panel = *config;
    framebuffer_size_bytes = (uint32_t)panel.width * panel.height * (uint32_t)sizeof(uint16_t);

    status = ltdc_configure_clock();
    if (status != HAL_OK)
    {
        return status;
    }

    panel.init_pins();
    /* Without it the LTDC cannot fetch the PSRAM framebuffer. */
    mcu_rif_grant_cpu_identity(RIF_MASTER_INDEX_LTDC1, RIF_RISC_PERIPH_INDEX_LTDCL1);
    __HAL_RCC_LTDC_CLK_ENABLE();
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();

    front_buffer = (uint16_t *)panel.framebuffer0;
    draw_buffer = (uint16_t *)panel.framebuffer1;
    /* Both framebuffers start white, cleaned to the memory the LTDC reads. */
    memset(front_buffer, 0xFF, framebuffer_size_bytes);
    ltdc_clean_framebuffer(front_buffer);
    memset(draw_buffer, 0xFF, framebuffer_size_bytes);
    ltdc_clean_framebuffer(draw_buffer);
    status = ltdc_configure_controller();
    if (status != HAL_OK)
    {
        __HAL_RCC_LTDC_FORCE_RESET();
        __HAL_RCC_LTDC_RELEASE_RESET();
        __HAL_RCC_LTDC_CLK_DISABLE();
        ltdc = (LTDC_HandleTypeDef){0};
        front_buffer = NULL;
        draw_buffer = NULL;
        return status;
    }

    initialized = true;
    return HAL_OK;
}

/**
 * @brief Get the framebuffer currently reserved for CPU rendering.
 * @return Writable RGB565 framebuffer, or NULL before initialization.
 */
uint16_t *drv_ltdc_get_draw_buffer(void)
{
    return initialized ? draw_buffer : NULL;
}

/**
 * @brief Get the configured display width.
 * @return Visible width in pixels, or 0 before initialization.
 */
uint16_t drv_ltdc_get_width(void)
{
    return initialized ? panel.width : 0U;
}

/**
 * @brief Get the configured display height.
 * @return Visible height in pixels, or 0 before initialization.
 */
uint16_t drv_ltdc_get_height(void)
{
    return initialized ? panel.height : 0U;
}

/**
 * @brief Show the completed draw buffer and acquire the next draw buffer.
 * @return A writable framebuffer after a completed vertical-blanking switch,
 *         or NULL if the display is unavailable or the LTDC operation fails.
 * @note The caller must stop modifying the current draw buffer before this call
 *       and serialize all draw and presentation operations.
 */
uint16_t *drv_ltdc_present(void)
{
    uint16_t *previous_front;
    HAL_StatusTypeDef status;
    uint32_t start_tick;

    if ((!initialized) || (draw_buffer == NULL))
    {
        return NULL;
    }

    ltdc_clean_framebuffer(draw_buffer);
    status = HAL_LTDC_SetAddress_NoReload(&ltdc,
                                         (uint32_t)(uintptr_t)draw_buffer,
                                         LTDC_LAYER_INDEX);
    if (status != HAL_OK)
    {
        return NULL;
    }

    status = HAL_LTDC_Reload(&ltdc, LTDC_RELOAD_VERTICAL_BLANKING);
    if (status != HAL_OK)
    {
        return NULL;
    }

    start_tick = HAL_GetTick();
    while ((ltdc.Instance->SRCR & LTDC_SRCR_VBR) != 0U)
    {
        if ((HAL_GetTick() - start_tick) >= LTDC_RELOAD_TIMEOUT_MS)
        {
            return NULL;
        }
    }

    previous_front = front_buffer;
    front_buffer = draw_buffer;
    draw_buffer = previous_front;
    return draw_buffer;
}
