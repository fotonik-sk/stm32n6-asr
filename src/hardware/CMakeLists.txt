# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Hardware layer (Level 0). Dependencies point downward only:
#   bsp -> drivers | components -> mcu
# mcu knows the SoC, drivers know a peripheral, components know an external IC,
# and only bsp knows the board.

# SoC: startup, CMSIS, HAL, core (MPU/caches), clock tree, resource isolation.
add_subdirectory(mcu)
# External IC drivers (NOR, PSRAM).
add_subdirectory(components)
# Board-agnostic SoC peripheral drivers (UART, LTDC, PDM capture, NPU).
add_subdirectory(drivers)
# STM32N6570-DK: board description, ST BSP, bsp_* modules, interrupt vectors.
add_subdirectory(bsp)
