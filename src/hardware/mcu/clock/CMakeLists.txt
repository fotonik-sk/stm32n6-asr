# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# STM32N6 clock tree (PLL1 system tree for the FSBL, PLL3 audio clock). The FSBL
# compiles mcu_clock.c directly from its own project.
add_library(mcu_clock mcu_clock.c)
target_include_directories(mcu_clock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mcu_clock PUBLIC stm32n6_hal)
