# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# ThreadX middleware integration for the STM32N6570-DK application.
# The upstream source remains a git submodule in ThreadX/; project-specific
# configuration and the Cortex-M55 low-level glue live next to this file.

set(THREADX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ThreadX)

file(GLOB THREADX_COMMON_SOURCES CONFIGURE_DEPENDS
    ${THREADX_ROOT}/common/src/*.c)
file(GLOB THREADX_PORT_ASM_SOURCES CONFIGURE_DEPENDS
    ${THREADX_ROOT}/ports/cortex_m55/gnu/src/*.S)

# Secure-stack helpers are not needed for this secure, single-mode image.
list(FILTER THREADX_PORT_ASM_SOURCES EXCLUDE REGEX "secure_stack")
list(FILTER THREADX_PORT_ASM_SOURCES EXCLUDE REGEX "tx_misra\\.S$")

add_library(threadx STATIC
    threadx.c
    threadx_initialize_low_level.S
    ${THREADX_COMMON_SOURCES}
    ${THREADX_PORT_ASM_SOURCES})

target_include_directories(threadx
    SYSTEM PUBLIC
        ${THREADX_ROOT}/common/inc
        ${THREADX_ROOT}/ports/cortex_m55/gnu/inc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(threadx PUBLIC
    TX_INCLUDE_USER_DEFINE_FILE
    TX_SINGLE_MODE_SECURE)
target_link_libraries(threadx PUBLIC stm32n6_hal)
