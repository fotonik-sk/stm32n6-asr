# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

add_library(drv_pdm_mdf drv_pdm_mdf.c)
target_include_directories(drv_pdm_mdf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(drv_pdm_mdf PUBLIC stm32n6_hal drv_pdm_dma PRIVATE mcu_clock)
