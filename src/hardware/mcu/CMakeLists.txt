# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# MCU / SoC layer: everything that belongs to the STM32N657 itself and knows
# nothing about the board.

# Startup, CMSIS SystemInit, CMSIS headers, linker scripts.
add_subdirectory(system)
# Vendor HAL + hal_conf + MSP.
add_subdirectory(hal)
# Cortex-M55 MPU and L1 caches.
add_subdirectory(core)
# Clock tree (PLL1 for the FSBL, PLL3 audio clock).
add_subdirectory(clock)
# Resource isolation (RIMC / RISC / RISAF).
add_subdirectory(rif)
