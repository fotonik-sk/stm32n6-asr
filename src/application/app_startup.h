/**
 * @file     app_startup.h
 * @brief    Start-up sequence of the firmware: which module starts when.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef APPLICATION_APP_STARTUP_H
#define APPLICATION_APP_STARTUP_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Create every module's RTOS objects and the startup thread.
 * @param[in] first_unused_memory Unused; all RTOS objects are static.
 * @note Called by ThreadX from tx_kernel_enter(), never by the application.
 */
void tx_application_define(void *first_unused_memory);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_APP_STARTUP_H */
