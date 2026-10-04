# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# NPU power, clocks and bus access (the runtime itself is middleware).
add_library(drv_npu drv_npu.c)
target_include_directories(drv_npu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(drv_npu PUBLIC stm32n6_hal PRIVATE mcu_rif)
