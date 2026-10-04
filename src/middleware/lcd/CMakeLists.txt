# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

add_library(mw_lcd mw_lcd.c)
target_include_directories(mw_lcd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
