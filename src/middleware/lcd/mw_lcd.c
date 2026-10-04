/**
 * @file     mw_lcd.c
 * @brief    RGB565 framebuffer text rendering and presentation.
 * @details  The glyph renderer is adapted from the legacy display_text module.
 *           Geometry, memory, and presentation are supplied by a backend.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mw_lcd.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief First character the font has a glyph for (space). */
#define MW_LCD_FONT_FIRST_CHAR  32U
/** @brief Last character the font has a glyph for ('~'). */
#define MW_LCD_FONT_LAST_CHAR   126U
/** @brief Pixels from the top of a text line down to the baseline, at scale 1. */
#define MW_LCD_FONT_ASCENT      44U
/** @brief Pixels from one text line to the next, at scale 1. */
#define MW_LCD_FONT_LINE_HEIGHT 55U

/*******************************************************************************/
/*                                PRIVATE TYPES                                */
/*******************************************************************************/

/** @brief Location and metrics of one packed-alpha font glyph. */
typedef struct
{
    uint16_t bitmap_offset; /**< First byte of the glyph in font_bitmap. */
    uint8_t width;          /**< Bitmap width in pixels; 0 for an empty glyph. */
    uint8_t height;         /**< Bitmap height in pixels; 0 for an empty glyph. */
    int8_t left;            /**< Bitmap left edge relative to the pen position. */
    int8_t top;             /**< Bitmap top edge relative to the baseline (negative is above). */
    uint8_t advance;        /**< Pen movement to the next character. */
} mw_lcd_glyph_t;

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

#include "mw_lcd_font_sans_48.inc"

/** @brief Active backend configuration and current hidden draw buffer. */
static mw_lcd_config_t active_config;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/** @brief Resolve printable ASCII to a glyph and replace unsupported bytes. */
static const mw_lcd_glyph_t *lcd_get_glyph(char character)
{
    uint8_t code = (uint8_t)character;

    if ((code < MW_LCD_FONT_FIRST_CHAR) || (code > MW_LCD_FONT_LAST_CHAR))
    {
        code = (uint8_t)'?';
    }

    return &font_glyphs[code - MW_LCD_FONT_FIRST_CHAR];
}

/** @brief Read one 4-bit glyph sample and expand it to 8-bit alpha. */
static uint8_t lcd_get_glyph_alpha(const mw_lcd_glyph_t *glyph,
                                   const uint8_t *bitmap,
                                   uint32_t row,
                                   uint32_t column)
{
    uint32_t pixel_index;
    uint8_t packed_alpha;
    uint8_t alpha4;

    if ((glyph == NULL) || (bitmap == NULL) || (row >= glyph->height) ||
        (column >= glyph->width))
    {
        return 0U;
    }

    /* Two pixels per byte, the even one in the high nibble. */
    pixel_index = (row * glyph->width) + column;
    packed_alpha = bitmap[pixel_index / 2U];
    alpha4 = ((pixel_index & 1U) == 0U)
                 ? (uint8_t)(packed_alpha >> 4U)
                 : (uint8_t)(packed_alpha & 0x0FU);
    /* x 17 maps 0..15 onto 0..255 (15 x 17 = 255). */
    return (uint8_t)(alpha4 * 17U);
}

/** @brief Scale a glyph sample with bilinear interpolation. */
static uint8_t lcd_sample_glyph_alpha(const mw_lcd_glyph_t *glyph,
                                      const uint8_t *bitmap,
                                      uint32_t output_x,
                                      uint32_t output_y,
                                      uint16_t scale)
{
    int32_t source_x_fp;
    int32_t source_y_fp;
    uint32_t x0;
    uint32_t x1;
    uint32_t y0;
    uint32_t y1;
    uint32_t fraction_x = 0U;
    uint32_t fraction_y = 0U;
    uint32_t alpha00;
    uint32_t alpha01;
    uint32_t alpha10;
    uint32_t alpha11;
    uint32_t alpha_top;
    uint32_t alpha_bottom;

    if ((glyph == NULL) || (bitmap == NULL) || (scale == 0U) ||
        (glyph->width == 0U) || (glyph->height == 0U))
    {
        return 0U;
    }
    if (scale == 1U)
    {
        return lcd_get_glyph_alpha(glyph, bitmap, output_y, output_x);
    }

    /* Map the centre of the output pixel back into the glyph, in 8.8 fixed
       point (256 = one source pixel): source = (output + 0.5) / scale - 0.5.
       Then take the two neighbours on each axis, clamped to the bitmap, and
       the fraction between them as the interpolation weight. */
    source_x_fp = (int32_t)((((uint32_t)output_x << 8U) + 128U) / scale) - 128;
    source_y_fp = (int32_t)((((uint32_t)output_y << 8U) + 128U) / scale) - 128;
    if (glyph->width <= 1U)
    {
        x0 = 0U;
        x1 = 0U;
    }
    else if (source_x_fp <= 0)
    {
        x0 = 0U;
        x1 = 1U;
    }
    else if ((uint32_t)source_x_fp >= (((uint32_t)glyph->width - 1U) << 8U))
    {
        x0 = glyph->width - 1U;
        x1 = x0;
    }
    else
    {
        x0 = (uint32_t)source_x_fp >> 8U;
        x1 = x0 + 1U;
        fraction_x = (uint32_t)source_x_fp & 0xFFU;
    }

    if (glyph->height <= 1U)
    {
        y0 = 0U;
        y1 = 0U;
    }
    else if (source_y_fp <= 0)
    {
        y0 = 0U;
        y1 = 1U;
    }
    else if ((uint32_t)source_y_fp >= (((uint32_t)glyph->height - 1U) << 8U))
    {
        y0 = glyph->height - 1U;
        y1 = y0;
    }
    else
    {
        y0 = (uint32_t)source_y_fp >> 8U;
        y1 = y0 + 1U;
        fraction_y = (uint32_t)source_y_fp & 0xFFU;
    }

    alpha00 = lcd_get_glyph_alpha(glyph, bitmap, y0, x0);
    alpha10 = lcd_get_glyph_alpha(glyph, bitmap, y0, x1);
    alpha01 = lcd_get_glyph_alpha(glyph, bitmap, y1, x0);
    alpha11 = lcd_get_glyph_alpha(glyph, bitmap, y1, x1);
    alpha_top = ((alpha00 * (256U - fraction_x)) +
                 (alpha10 * fraction_x) + 128U) >> 8U;
    alpha_bottom = ((alpha01 * (256U - fraction_x)) +
                    (alpha11 * fraction_x) + 128U) >> 8U;
    return (uint8_t)(((alpha_top * (256U - fraction_y)) +
                      (alpha_bottom * fraction_y) + 128U) >> 8U);
}

/** @brief Alpha-blend two RGB565 pixels. */
static uint16_t lcd_blend_rgb565(uint16_t background, uint16_t foreground, uint8_t alpha)
{
    uint32_t source_red;
    uint32_t source_green;
    uint32_t source_blue;
    uint32_t destination_red;
    uint32_t destination_green;
    uint32_t destination_blue;
    uint32_t inverse_alpha;
    uint32_t output_red;
    uint32_t output_green;
    uint32_t output_blue;

    if (alpha == 255U)
    {
        return foreground;
    }
    if (alpha == 0U)
    {
        return background;
    }

    /* Split RGB565 into its 5-6-5-bit channels and blend each one:
       out = (fg * alpha + bg * (255 - alpha)) / 255, rounded. */
    source_red = (foreground >> 11U) & 0x1FU;
    source_green = (foreground >> 5U) & 0x3FU;
    source_blue = foreground & 0x1FU;
    destination_red = (background >> 11U) & 0x1FU;
    destination_green = (background >> 5U) & 0x3FU;
    destination_blue = background & 0x1FU;
    inverse_alpha = 255U - alpha;
    output_red = ((source_red * alpha) +
                  (destination_red * inverse_alpha) + 127U) / 255U;
    output_green = ((source_green * alpha) +
                    (destination_green * inverse_alpha) + 127U) / 255U;
    output_blue = ((source_blue * alpha) +
                   (destination_blue * inverse_alpha) + 127U) / 255U;
    return (uint16_t)((output_red << 11U) | (output_green << 5U) | output_blue);
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

mw_lcd_status_t mw_lcd_init(const mw_lcd_config_t *config)
{
    if ((config == NULL) || (config->width == 0U) || (config->height == 0U) ||
        (config->stride_pixels < config->width) || (config->draw_buffer == NULL) ||
        (config->present_callback == NULL) ||
        (config->stride_pixels > (UINT32_MAX / config->height)))
    {
        return MW_LCD_ERROR;
    }

    active_config = *config;
    return MW_LCD_OK;
}

void mw_lcd_clear(uint16_t color)
{
    uint32_t row;
    uint32_t column;

    if (active_config.draw_buffer == NULL)
    {
        return;
    }

    for (row = 0U; row < active_config.height; ++row)
    {
        uint32_t row_offset = row * active_config.stride_pixels;

        for (column = 0U; column < active_config.width; ++column)
        {
            active_config.draw_buffer[row_offset + column] = color;
        }
    }
}

void mw_lcd_draw_char(uint16_t x,
                      uint16_t y,
                      char character,
                      uint16_t scale,
                      uint16_t color,
                      bool antialias)
{
    const mw_lcd_glyph_t *glyph;
    const uint8_t *bitmap;
    int32_t glyph_x;
    int32_t glyph_y;
    int32_t baseline_y;
    uint32_t output_width;
    uint32_t output_height;
    uint32_t output_y;
    uint32_t output_x;

    if ((active_config.draw_buffer == NULL) || (scale == 0U))
    {
        return;
    }

    glyph = lcd_get_glyph(character);
    if ((glyph->width == 0U) || (glyph->height == 0U))
    {
        return;
    }
    bitmap = &font_bitmap[glyph->bitmap_offset];

    baseline_y = (int32_t)y + ((int32_t)MW_LCD_FONT_ASCENT * (int32_t)scale);
    glyph_x = (int32_t)x + ((int32_t)glyph->left * (int32_t)scale);
    glyph_y = baseline_y + ((int32_t)glyph->top * (int32_t)scale);
    output_width = (uint32_t)glyph->width * scale;
    output_height = (uint32_t)glyph->height * scale;
    for (output_y = 0U; output_y < output_height; ++output_y)
    {
        int32_t pixel_y = glyph_y + (int32_t)output_y;

        if ((pixel_y < 0) || (pixel_y >= (int32_t)active_config.height))
        {
            continue;
        }
        for (output_x = 0U; output_x < output_width; ++output_x)
        {
            int32_t pixel_x = glyph_x + (int32_t)output_x;
            uint8_t alpha;
            uint32_t pixel_index;

            if ((pixel_x < 0) || (pixel_x >= (int32_t)active_config.width))
            {
                continue;
            }

            alpha = lcd_sample_glyph_alpha(glyph, bitmap, output_x, output_y, scale);
            pixel_index = ((uint32_t)pixel_y * active_config.stride_pixels) +
                          (uint32_t)pixel_x;
            if (antialias)
            {
                active_config.draw_buffer[pixel_index] =
                    lcd_blend_rgb565(active_config.draw_buffer[pixel_index], color, alpha);
            }
            else if (alpha >= 128U)
            {
                active_config.draw_buffer[pixel_index] = color;
            }
        }
    }
}

void mw_lcd_draw_string(uint16_t x,
                        uint16_t y,
                        const char *text,
                        uint16_t scale,
                        uint16_t color,
                        bool antialias)
{
    uint32_t cursor_x;
    uint32_t cursor_y;
    uint32_t line_step;

    if ((active_config.draw_buffer == NULL) || (text == NULL) || (scale == 0U))
    {
        return;
    }

    cursor_x = x;
    cursor_y = y;
    line_step = mw_lcd_get_line_height(scale);
    while (*text != '\0')
    {
        const mw_lcd_glyph_t *glyph;

        if (*text == '\n')
        {
            cursor_x = x;
            cursor_y += line_step;
            ++text;
            continue;
        }

        mw_lcd_draw_char((uint16_t)cursor_x,
                         (uint16_t)cursor_y,
                         *text,
                         scale,
                         color,
                         antialias);
        glyph = lcd_get_glyph(*text);
        cursor_x += (uint32_t)glyph->advance * scale;
        ++text;
    }
}

mw_lcd_status_t mw_lcd_draw_image(int32_t x,
                                  int32_t y,
                                  const mw_lcd_image_t *image)
{
    int32_t source_x = 0;
    int32_t source_y = 0;
    int32_t copy_width;
    int32_t copy_height;
    int32_t row;

    if ((image == NULL) || (image->pixels == NULL) || (image->width == 0U) ||
        (image->height == 0U) || (active_config.draw_buffer == NULL))
    {
        return MW_LCD_ERROR;
    }

    if (x < 0)
    {
        source_x = -x;
        x = 0;
    }
    if (y < 0)
    {
        source_y = -y;
        y = 0;
    }
    copy_width = (int32_t)image->width - source_x;
    copy_height = (int32_t)image->height - source_y;
    if (copy_width > ((int32_t)active_config.width - x))
    {
        copy_width = (int32_t)active_config.width - x;
    }
    if (copy_height > ((int32_t)active_config.height - y))
    {
        copy_height = (int32_t)active_config.height - y;
    }
    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return MW_LCD_OK;
    }

    for (row = 0; row < copy_height; ++row)
    {
        const uint16_t *source =
            &image->pixels[((uint32_t)(source_y + row) * image->width) + (uint32_t)source_x];
        uint16_t *destination =
            &active_config.draw_buffer[((uint32_t)(y + row) *
                                        active_config.stride_pixels) +
                                       (uint32_t)x];

        (void)memcpy(destination, source, (size_t)copy_width * sizeof(uint16_t));
    }
    return MW_LCD_OK;
}

mw_lcd_status_t mw_lcd_draw_image_centered(const mw_lcd_image_t *image)
{
    if (image == NULL)
    {
        return MW_LCD_ERROR;
    }

    return mw_lcd_draw_image(((int32_t)active_config.width - (int32_t)image->width) / 2,
                             ((int32_t)active_config.height - (int32_t)image->height) / 2,
                             image);
}

uint32_t mw_lcd_get_line_height(uint16_t scale)
{
    return MW_LCD_FONT_LINE_HEIGHT * scale;
}

uint32_t mw_lcd_measure_string_width(const char *text, uint16_t scale)
{
    uint32_t cursor_x = 0U;
    uint32_t maximum_width = 0U;

    if ((text == NULL) || (scale == 0U))
    {
        return 0U;
    }

    while (*text != '\0')
    {
        if (*text == '\n')
        {
            if (cursor_x > maximum_width)
            {
                maximum_width = cursor_x;
            }
            cursor_x = 0U;
        }
        else
        {
            cursor_x += (uint32_t)lcd_get_glyph(*text)->advance * scale;
        }
        ++text;
    }

    return (cursor_x > maximum_width) ? cursor_x : maximum_width;
}

uint16_t mw_lcd_get_width(void)
{
    return active_config.width;
}

uint16_t mw_lcd_get_height(void)
{
    return active_config.height;
}

uint16_t mw_lcd_rgb888_to_rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
    return (uint16_t)((((uint16_t)red & 0xF8U) << 8U) |
                      (((uint16_t)green & 0xFCU) << 3U) |
                      ((uint16_t)blue >> 3U));
}

mw_lcd_status_t mw_lcd_present(void)
{
    uint16_t *next_buffer;

    if ((active_config.draw_buffer == NULL) || (active_config.present_callback == NULL))
    {
        return MW_LCD_ERROR;
    }

    next_buffer = active_config.present_callback(active_config.callback_context);
    if (next_buffer == NULL)
    {
        return MW_LCD_ERROR;
    }

    active_config.draw_buffer = next_buffer;
    return MW_LCD_OK;
}
