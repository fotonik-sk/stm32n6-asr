/**
 * @file     startup_logo.h
 * @brief    Startup picture shown before the transcript takes over the LCD.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef APPLICATION_DISPLAY_STARTUP_LOGO_H
#define APPLICATION_DISPLAY_STARTUP_LOGO_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Draw the startup logo full screen and present it.
 * @return Zero on success, non-zero when drawing or presentation failed.
 * @pre mw_lcd is initialized, and no other thread draws yet.
 */
uint32_t startup_logo_show(void);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_DISPLAY_STARTUP_LOGO_H */
