# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# ---------------------------------------------------------------------------
# Device/board definition for STM32N657X0H3QU (Cortex-M55) on STM32N6570-DK.
# Sets the STM32N6_* variables that the top-level CMakeLists turns into
# compile definitions and compile/link flags.
# ---------------------------------------------------------------------------

set(STM32N6_BOARD_DEFINE BOARD_STM32N6570_DK)

# HAL + CMSIS device selectors (USE_HAL_DRIVER is added globally).
set(STM32N6_DEVICE_DEFINES STM32N657xx STM32N6xx STM32N6)

# CPU/FPU flags -- valid for both C and ASM.
set(STM32N6_CPU_FLAGS
    -mcpu=cortex-m55
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard)

# First-party compiler options safe for every language.
set(STM32N6_COMPILE_FLAGS
    -ffunction-sections
    -fdata-sections
    -fsigned-char
    -fmessage-length=0)

# C optimisation level (overridable from a preset / -D).
set(STM32_C_OPTIMIZATION "-O0" CACHE STRING
    "C optimization flag (-O0, -O1, -O2, -O3, -Os, -Og)")
