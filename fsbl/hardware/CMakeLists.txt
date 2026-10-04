# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# FSBL interrupt handlers.
add_library(fsbl_it OBJECT stm32n6xx_it.c)
target_include_directories(fsbl_it PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(fsbl_it PRIVATE stm32n6_hal)

# System (startup, CMSIS, AXISRAM2 linker script)
add_subdirectory(system)
# MCU HAL (vendor HAL + conf + MSP)
add_subdirectory(mcu_hal)
