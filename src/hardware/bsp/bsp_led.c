/**
 * @file     bsp_led.c
 * @brief    Board user LEDs over the ST BSP; implements bsp_led.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "bsp_led.h"

#include "stm32n6570_discovery.h"

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Translate a board LED into the ST BSP identifier.
 * @param[in] led Board LED.
 * @return Matching ST BSP LED.
 */
static Led_TypeDef to_st_bsp(bsp_led_t led)
{
    return (BSP_LED_RED == led) ? LED_RED : LED_GREEN;
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void bsp_led_init(void)
{
    (void)BSP_LED_Init(LED_GREEN);
    (void)BSP_LED_Init(LED_RED);
}

void bsp_led_on(bsp_led_t led)
{
    (void)BSP_LED_On(to_st_bsp(led));
}

void bsp_led_off(bsp_led_t led)
{
    (void)BSP_LED_Off(to_st_bsp(led));
}

void bsp_led_toggle(bsp_led_t led)
{
    (void)BSP_LED_Toggle(to_st_bsp(led));
}
