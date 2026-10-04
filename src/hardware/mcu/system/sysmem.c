/**
 * @file     sysmem.c
 * @brief    Bounded C library heap; implements sysmem.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "sysmem.h"

#include <errno.h>
#include <stdint.h>

/*******************************************************************************/
/*                              EXTERNAL VARIABLES                             */
/*******************************************************************************/

/** @brief First byte of the .heap section (STM32N657XX_XIP.ld). */
extern uint8_t __heap_start[];
/** @brief First byte past the .heap section (STM32N657XX_XIP.ld). */
extern uint8_t __heap_end[];

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Current heap break; NULL until the first call. */
static uint8_t *heap_break;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void *_sbrk(ptrdiff_t increment)
{
    uint8_t *previous;

    if (heap_break == NULL)
    {
        heap_break = __heap_start;
    }

    if ((increment > (__heap_end - heap_break)) ||
        (increment < (__heap_start - heap_break)))
    {
        errno = ENOMEM;
        return (void *)-1;
    }

    previous = heap_break;
    heap_break += increment;
    return previous;
}
