/**
 * @file     sysmem.h
 * @brief    Bounded C library heap: the newlib _sbrk() hook.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HARDWARE_MCU_SYSTEM_SYSMEM_H
#define HARDWARE_MCU_SYSTEM_SYSMEM_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Move the C library heap break by @p increment bytes.
 * @return The previous break, or (void *)-1 with errno = ENOMEM when the
 *         request would leave the heap section.
 */
void *_sbrk(ptrdiff_t increment);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_MCU_SYSTEM_SYSMEM_H */
