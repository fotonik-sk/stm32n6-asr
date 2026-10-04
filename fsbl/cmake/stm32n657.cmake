# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# ---------------------------------------------------------------------------
# Device definition for the FSBL (STM32N657X0H3QU, Cortex-M55). No board define:
# the FSBL only talks to the external flash through the External Memory Manager.
# NO_OTP_FUSE is added in the top-level CMakeLists and must never be removed.
# ---------------------------------------------------------------------------
set(STM32N6_DEVICE_DEFINES STM32N657xx STM32N6xx STM32N6)

set(STM32N6_CPU_FLAGS
    -mcpu=cortex-m55
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard)

set(STM32N6_COMPILE_FLAGS
    -ffunction-sections
    -fdata-sections
    -fsigned-char
    -fmessage-length=0)

set(STM32_C_OPTIMIZATION "-O0" CACHE STRING
    "C optimization flag (-O0, -O1, -O2, -O3, -Os, -Og)")
