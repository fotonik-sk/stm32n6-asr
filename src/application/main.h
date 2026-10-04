/**
 * @file     main.h
 * @brief    Application entry header.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef APPLICATION_MAIN_H
#define APPLICATION_MAIN_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "stm32n6xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void error_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_MAIN_H */
