/**
 * @file     tx_user.h
 * @brief    Project ThreadX configuration.
 * @details  Keep application policy here, not in the upstream submodule.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef STM32N6_THREADX_TX_USER_H
#define STM32N6_THREADX_TX_USER_H

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

#define TX_MAX_PRIORITIES                 32
#define TX_TIMER_TICKS_PER_SECOND         1000UL
#define TX_TIMER_PROCESS_IN_ISR

#endif /* STM32N6_THREADX_TX_USER_H */
