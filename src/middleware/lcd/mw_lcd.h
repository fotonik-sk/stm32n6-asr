/**
 * @file     mw_lcd.h
 * @brief    RGB565 framebuffer drawing API for the LCD middleware.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef MIDDLEWARE_LCD_MW_LCD_H
#define MIDDLEWARE_LCD_MW_LCD_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

#define MW_LCD_COLOR_BLACK 0x0000U
#define MW_LCD_COLOR_WHITE 0xFFFFU

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief LCD middleware operation result. */
typedef enum
{
    MW_LCD_OK = 0, /**< Operation completed successfully. */
    MW_LCD_ERROR   /**< Middleware is not ready or its backend operation failed. */
} mw_lcd_status_t;

/**
 * @brief Present the backend's current frame and acquire the next draw buffer.
 * @param[in,out] context Opaque display-backend context.
 * @return Writable buffer for the next frame, or NULL when presentation fails.
 */
typedef uint16_t *(*mw_lcd_present_callback_t)(void *context);

/** @brief Display-independent framebuffer configuration. */
typedef struct
{
    uint16_t width;                            /**< Visible width in pixels. */
    uint16_t height;                           /**< Visible height in pixels. */
    uint32_t stride_pixels;                    /**< Distance between row starts. */
    uint16_t *draw_buffer;                     /**< Initial writable RGB565 buffer. */
    mw_lcd_present_callback_t present_callback; /**< Frame submission callback. */
    void *callback_context;                    /**< Opaque callback argument. */
} mw_lcd_config_t;

/** @brief Uncompressed RGB565 image with tightly packed rows. */
typedef struct
{
    uint16_t width;         /**< Width in pixels. */
    uint16_t height;        /**< Height in pixels. */
    const uint16_t *pixels; /**< width * height RGB565 pixels, row-major. */
} mw_lcd_image_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Attach the renderer to a display backend and its initial draw buffer.
 * @param[in] config Display geometry, row stride, draw buffer, and presentation
 *            callback. The structure is copied and may be released after return.
 * @retval MW_LCD_OK Middleware is ready, including after a repeated call.
 * @retval MW_LCD_ERROR Configuration is NULL, incomplete, or cannot be indexed.
 * @pre The backend and initial draw buffer are ready for use.
 * @note Rendering and presentation calls must be serialized by the application.
 */
mw_lcd_status_t mw_lcd_init(const mw_lcd_config_t *config);

/**
 * @brief Fill the complete hidden framebuffer with one RGB565 color.
 * @param[in] color RGB565 fill color.
 */
void mw_lcd_clear(uint16_t color);

/**
 * @brief Draw one glyph into the hidden framebuffer.
 * @param[in] x Left text origin in pixels.
 * @param[in] y Top text origin in pixels.
 * @param[in] character Printable ASCII character; unsupported bytes render as '?'.
 * @param[in] scale Integer font scale; zero draws nothing.
 * @param[in] color RGB565 foreground color.
 * @param[in] antialias true to alpha-blend the edges, false to threshold them.
 */
void mw_lcd_draw_char(uint16_t x,
                      uint16_t y,
                      char character,
                      uint16_t scale,
                      uint16_t color,
                      bool antialias);

/**
 * @brief Draw a newline-aware string into the hidden framebuffer.
 * @param[in] x Left origin of every line in pixels.
 * @param[in] y Top origin of the first line in pixels.
 * @param[in] text Null-terminated text; NULL draws nothing.
 * @param[in] scale Integer font scale; zero draws nothing.
 * @param[in] color RGB565 foreground color.
 * @param[in] antialias true to alpha-blend the edges, false to threshold them.
 */
void mw_lcd_draw_string(uint16_t x,
                        uint16_t y,
                        const char *text,
                        uint16_t scale,
                        uint16_t color,
                        bool antialias);

/**
 * @brief Copy an RGB565 image into the hidden framebuffer.
 * @param[in] x Left edge in pixels; negative crops the image's left, parts
 *            beyond the screen are clipped.
 * @param[in] y Top edge in pixels; negative crops the image's top, parts beyond
 *            the screen are clipped.
 * @param[in] image Image to draw.
 * @retval MW_LCD_OK The image was drawn, or lies entirely off screen.
 * @retval MW_LCD_ERROR The image is invalid or the middleware is not ready.
 * @note The frame becomes visible on the next mw_lcd_present().
 */
mw_lcd_status_t mw_lcd_draw_image(int32_t x,
                                  int32_t y,
                                  const mw_lcd_image_t *image);

/**
 * @brief Draw an RGB565 image centred on the screen.
 * @param[in] image Image to draw; a larger one is cropped around its centre.
 * @return As mw_lcd_draw_image().
 */
mw_lcd_status_t mw_lcd_draw_image_centered(const mw_lcd_image_t *image);

/**
 * @brief Get the line advance for the bundled 48-pixel font.
 * @param[in] scale Integer font scale.
 * @return Line height in pixels, or zero for scale zero.
 */
uint32_t mw_lcd_get_line_height(uint16_t scale);

/**
 * @brief Measure the widest line in a newline-aware string.
 * @param[in] text Null-terminated text.
 * @param[in] scale Integer font scale.
 * @return Width in pixels, or zero for NULL text or scale zero.
 */
uint32_t mw_lcd_measure_string_width(const char *text, uint16_t scale);

/**
 * @brief Return the configured visible width.
 * @return Width in pixels, or zero before successful initialization.
 */
uint16_t mw_lcd_get_width(void);

/**
 * @brief Return the configured visible height.
 * @return Height in pixels, or zero before successful initialization.
 */
uint16_t mw_lcd_get_height(void);

/**
 * @brief Convert 8-bit RGB components to RGB565.
 * @param[in] red Red component.
 * @param[in] green Green component.
 * @param[in] blue Blue component.
 * @return Packed RGB565 color.
 */
uint16_t mw_lcd_rgb888_to_rgb565(uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Display the completed hidden frame and start rendering into the other one.
 * @retval MW_LCD_OK Backend accepted the frame and returned the next draw buffer.
 * @retval MW_LCD_ERROR Middleware is not ready or its backend returned NULL.
 */
mw_lcd_status_t mw_lcd_present(void);

#ifdef __cplusplus
}
#endif

#endif /* MIDDLEWARE_LCD_MW_LCD_H */
