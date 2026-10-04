/**
 * @file     version.h
 * @brief    Application version information.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef APPLICATION_VERSION_H
#define APPLICATION_VERSION_H

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

#define VERSION_MAJOR     0
#define VERSION_MINOR     1
#define VERSION_REVISION  0

#define VERSION_STRINGIFY_(x) #x
#define VERSION_STRINGIFY(x)  VERSION_STRINGIFY_(x)

#define VERSION_STRING \
  VERSION_STRINGIFY(VERSION_MAJOR) "." \
  VERSION_STRINGIFY(VERSION_MINOR) "." \
  VERSION_STRINGIFY(VERSION_REVISION)

#endif /* APPLICATION_VERSION_H */
