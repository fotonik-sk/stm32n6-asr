/**
 * @file     transcript.h
 * @brief    The recognized text as it appears on the LCD.
 * @details  Final text in black, the tentative tail in grey after it,
 *           word-wrapped, newest lines on screen; the oldest words drop out
 *           when the buffer fills. Only the display thread calls it, so there
 *           is no locking.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef APPLICATION_DISPLAY_TRANSCRIPT_H
#define APPLICATION_DISPLAY_TRANSCRIPT_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "mw_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/** @brief Room for the tentative tail, terminator included; longer text is cut. */
#define TRANSCRIPT_TENTATIVE_CAPACITY 320U

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Append final text to the transcript.
 * @param[in] text Characters to append; need not be terminated.
 * @param[in] length Number of characters in @p text.
 */
void transcript_append(const char *text, uint32_t length);

/**
 * @brief Replace the tentative tail.
 * @param[in] text New tail, NUL-terminated; may be empty.
 */
void transcript_set_tentative(const char *text);

/**
 * @brief Tell whether the text holds at least one non-space character.
 * @return Non-zero once there is something worth drawing.
 */
int transcript_has_text(void);

/**
 * @brief Draw the newest lines of the text on a white screen and present them.
 * @return MW_LCD_OK, or the mw_lcd error of the presentation.
 * @pre mw_lcd is initialized.
 */
mw_lcd_status_t transcript_draw(void);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_DISPLAY_TRANSCRIPT_H */
