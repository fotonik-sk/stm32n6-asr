# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

add_library(logger logger.c)
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(logger PUBLIC threadx PRIVATE stm32n6_hal)
