/**
 * @file     bsp_led.h
 * @brief    Board user LEDs.
 * @details  Thin abstraction over the ST BSP LED API so the application can
 *           drive LEDs without depending on vendor BSP types directly.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_BSP_BSP_LED_H
#define HARDWARE_BSP_BSP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief User LEDs of the board. */
typedef enum
{
    BSP_LED_GREEN = 0, /**< PO1 on STM32N6570-DK */
    BSP_LED_RED   = 1  /**< PG10 on STM32N6570-DK */
} bsp_led_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/** @brief Configure every user LED as an output, switched off. */
void bsp_led_init(void);

/**
 * @brief Switch one LED on.
 * @param[in] led LED to drive.
 */
void bsp_led_on(bsp_led_t led);

/**
 * @brief Switch one LED off.
 * @param[in] led LED to drive.
 */
void bsp_led_off(bsp_led_t led);

/**
 * @brief Invert one LED.
 * @param[in] led LED to drive.
 */
void bsp_led_toggle(bsp_led_t led);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_BSP_BSP_LED_H */
