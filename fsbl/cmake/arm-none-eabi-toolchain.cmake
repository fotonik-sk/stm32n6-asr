# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# ---------------------------------------------------------------------------
# GNU Arm Embedded (arm-none-eabi) toolchain file for STM32N6 (Cortex-M55).
#
# The compiler is taken from PATH. If it is not there, the build falls back to
# a local STM32CubeIDE installation (gnu-tools-for-stm32 plugin).
# ---------------------------------------------------------------------------
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m55)

# Build static libraries during the compiler check (we cannot link a full
# firmware image without the linker script).
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TOOLCHAIN_PREFIX "arm-none-eabi" CACHE STRING "GNU Arm Embedded toolchain prefix")

file(GLOB _stm32_gnu_bins
  "$ENV{HOME}/st/stm32cubeide_*/plugins/*gnu-tools-for-stm32*/tools/bin"
  "$ENV{HOME}/STM32CubeIDE/plugins/*gnu-tools-for-stm32*/tools/bin"
  "/opt/st/stm32cubeide_*/plugins/*gnu-tools-for-stm32*/tools/bin")

find_program(_STM32_C_COMPILER   NAMES ${TOOLCHAIN_PREFIX}-gcc     PATHS ${_stm32_gnu_bins} REQUIRED)
find_program(_STM32_ASM_COMPILER NAMES ${TOOLCHAIN_PREFIX}-gcc     PATHS ${_stm32_gnu_bins} REQUIRED)
find_program(CMAKE_OBJCOPY       NAMES ${TOOLCHAIN_PREFIX}-objcopy PATHS ${_stm32_gnu_bins} REQUIRED)
find_program(CMAKE_OBJDUMP       NAMES ${TOOLCHAIN_PREFIX}-objdump PATHS ${_stm32_gnu_bins} REQUIRED)
find_program(CMAKE_SIZE          NAMES ${TOOLCHAIN_PREFIX}-size    PATHS ${_stm32_gnu_bins} REQUIRED)
find_program(CMAKE_AR            NAMES ${TOOLCHAIN_PREFIX}-ar      PATHS ${_stm32_gnu_bins} REQUIRED)
find_program(CMAKE_RANLIB        NAMES ${TOOLCHAIN_PREFIX}-ranlib  PATHS ${_stm32_gnu_bins} REQUIRED)

set(CMAKE_C_COMPILER   "${_STM32_C_COMPILER}")
set(CMAKE_ASM_COMPILER "${_STM32_ASM_COMPILER}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
