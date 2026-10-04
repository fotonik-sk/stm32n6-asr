# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.22)

# Standalone First Stage Boot Loader project. Builds independently of the
# application (its own toolchain/presets/build dir) but lives in the same git
# repo and shares the same conventions. See fsbl/README.md.
project(stm32n6_fsbl
    LANGUAGES C ASM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(firmware_image)
include(stm32n657)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# SAFETY: NO_OTP_FUSE switches off the stock Template_FSBL_XIP VDDIO3 HSLV OTP
# write. main.c no longer carries that code at all; the define stays so anything
# re-imported from the template remains off.
add_compile_definitions(${STM32N6_DEVICE_DEFINES} NO_OTP_FUSE)
add_compile_options(${STM32N6_CPU_FLAGS} ${STM32N6_COMPILE_FLAGS})
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

add_subdirectory(hardware)
add_subdirectory(middleware)
add_subdirectory(application)
