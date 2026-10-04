# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

add_library(drv_uart drv_uart.c)
target_include_directories(drv_uart PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(drv_uart PUBLIC stm32n6_hal)
