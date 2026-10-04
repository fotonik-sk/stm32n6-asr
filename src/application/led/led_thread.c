/**
 * @file     led_thread.c
 * @brief    Heartbeat thread that toggles the green LED every 500 ms.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "led_thread.h"

#include "app_threads.h"
#include "bsp_led.h"
#include "tx_api.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief Half a blink period: 500 ms at 1000 ticks per second. */
#define LED_TOGGLE_TICKS  500U

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Control block of the LED thread. */
static TX_THREAD led_thread;

/** @brief Private stack of the LED thread. */
static ULONG led_thread_stack[APP_LED_THREAD_STACK_BYTES / sizeof(ULONG)];

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/** @brief Toggle the green LED forever. */
static void led_thread_entry(ULONG argument)
{
  (void)argument;

  for (;;)
  {
    bsp_led_toggle(BSP_LED_GREEN);
    (void)tx_thread_sleep(LED_TOGGLE_TICKS);
  }
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

uint32_t led_thread_create(void)
{
  return (uint32_t)tx_thread_create(&led_thread,
                                    "led_thread",
                                    led_thread_entry,
                                    0U,
                                    led_thread_stack,
                                    sizeof(led_thread_stack),
                                    APP_LED_THREAD_PRIORITY,
                                    APP_LED_THREAD_PRIORITY,
                                    TX_NO_TIME_SLICE,
                                    TX_DONT_START);
}

uint32_t led_thread_start(void)
{
  return (uint32_t)tx_thread_resume(&led_thread);
}
