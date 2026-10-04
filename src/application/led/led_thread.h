/**
 * @file     led_thread.h
 * @brief    Heartbeat thread: blinks the green LED while the firmware runs.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef APPLICATION_LED_LED_THREAD_H
#define APPLICATION_LED_LED_THREAD_H

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
 * @brief Create the LED thread, suspended.
 * @return TX_SUCCESS, or the ThreadX error.
 * @pre Call from tx_application_define(), before the scheduler starts.
 */
uint32_t led_thread_create(void);

/**
 * @brief Resume the LED thread; the LED blinks from here on.
 * @return TX_SUCCESS, or the ThreadX error.
 * @pre bsp_init() has configured the LEDs.
 */
uint32_t led_thread_start(void);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_LED_LED_THREAD_H */
