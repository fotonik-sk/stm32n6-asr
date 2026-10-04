# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# SoC peripheral drivers. None of them knows the board: instances, pins, timings
# and addresses come from the BSP through each driver's config structure.
add_subdirectory(uart)
add_subdirectory(ltdc)
add_subdirectory(pdm)
add_subdirectory(npu)
