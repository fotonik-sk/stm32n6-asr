/**
 * @file     boards.h
 * @brief    Board selection (hardware layer).
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_BSP_BOARDS_H
#define HARDWARE_BSP_BOARDS_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#if defined(BOARD_STM32N6570_DK)
#include "stm32n6570dk.h"
#else
#error "Board is not defined (expected BOARD_STM32N6570_DK)"
#endif

#endif /* HARDWARE_BSP_BOARDS_H */
