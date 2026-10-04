# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Resource isolation policy (RIMC master identities, RISC, RISAF firewalls).
add_library(mcu_rif mcu_rif.c)
target_include_directories(mcu_rif PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mcu_rif PUBLIC stm32n6_hal)
