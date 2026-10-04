# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.22)

project(stm32n6_base
    LANGUAGES C ASM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(firmware_image)
include(stm32n657)

message(VERBOSE "Use toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

# Project-wide setup -----------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Definitions and flags shared by every target. USE_HAL_DRIVER is NOT global: it
# is a public requirement of the stm32n6_hal target, so the CMSIS-only `system`
# target does not pull in the HAL.
add_compile_definitions(${STM32N6_DEVICE_DEFINES} ${STM32N6_BOARD_DEFINE})
add_compile_options(${STM32N6_CPU_FLAGS} ${STM32N6_COMPILE_FLAGS})

# C-only: -mcmse (device boots in the Arm TrustZone secure state; the CMSIS
# system files configure SAU/SCB_NS, visible to the compiler only in CMSE mode),
# optimisation and Debug-only symbols. ASM gets preprocessing.
add_compile_options(
    $<$<COMPILE_LANGUAGE:C>:-mcmse>
    $<$<COMPILE_LANGUAGE:C>:${STM32_C_OPTIMIZATION}>
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g3>
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:ASM>>:-g3>
    $<$<COMPILE_LANGUAGE:ASM>:-x>
    $<$<COMPILE_LANGUAGE:ASM>:assembler-with-cpp>)
add_compile_options(--specs=nano.specs)

add_link_options(
    ${STM32N6_CPU_FLAGS}
    --specs=nosys.specs
    --specs=nano.specs
    -static
    -Wl,--gc-sections
    -Wl,--no-warn-rwx-segments
    -Wl,--print-memory-usage)

# The project builds exactly one image: the XIP application the FSBL starts in
# place from external NOR (STM32N657XX_XIP.ld, preset "default").
add_subdirectory(src)
