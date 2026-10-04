/**
 * @file     display_thread.h
 * @brief    Display thread: the startup logo, then the recognized transcript.
 * @details  The asr_engine text port copies each update into a mailbox and
 *           wakes this thread, which redraws the transcript; the network thread
 *           never waits for drawing.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef APPLICATION_DISPLAY_DISPLAY_THREAD_H
#define APPLICATION_DISPLAY_DISPLAY_THREAD_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "asr_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Create the display thread and its mailbox, suspended.
 * @return TX_SUCCESS, or the ThreadX error of the failing step.
 * @pre Call from tx_application_define(), before the scheduler starts.
 */
uint32_t display_thread_create(void);

/**
 * @brief Bring up the LCD, show the startup logo, then resume the thread.
 * @return TX_SUCCESS, or non-zero on failure; the reason is logged.
 * @pre The logger is initialized. Afterwards only this thread draws (mw_lcd is
 *      not thread safe).
 * @note The logo stays until the first word is recognized.
 */
uint32_t display_thread_start(void);

/**
 * @brief Return the text port that feeds the display; pass it to asr_engine_start().
 * @return A pointer valid for the life of the program.
 */
const asr_text_port_t *display_text_port_get(void);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_DISPLAY_DISPLAY_THREAD_H */
