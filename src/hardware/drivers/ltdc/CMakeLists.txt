# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

add_library(drv_ltdc drv_ltdc.c)
target_include_directories(drv_ltdc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(drv_ltdc PUBLIC stm32n6_hal PRIVATE mcu_rif)
