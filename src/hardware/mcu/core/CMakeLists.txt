# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Cortex-M55 core setup (MPU regions, L1 caches).
add_library(mcu_core mcu_core.c)
target_include_directories(mcu_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mcu_core PUBLIC stm32n6_hal)
