# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Implements the reader side of ../drv_pdm.h, so it sees the public header.
add_library(drv_pdm_ring drv_pdm_ring.c)
target_include_directories(drv_pdm_ring
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(drv_pdm_ring PUBLIC stm32n6_hal)
