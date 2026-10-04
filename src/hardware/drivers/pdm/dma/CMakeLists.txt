# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Implements drv_pdm_irq_handler() of ../drv_pdm.h, so it sees the public header.
add_library(drv_pdm_dma drv_pdm_dma.c)
target_include_directories(drv_pdm_dma
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(drv_pdm_dma PUBLIC stm32n6_hal)
