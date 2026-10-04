# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# STM32N6 system
add_library(system OBJECT startup_stm32n657xx.s system_stm32n6xx.c sysmem.c)
target_include_directories(system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(system PUBLIC stm32n6_cmsis)

# ARM CMSIS core.
add_library(cmsis INTERFACE)
target_include_directories(cmsis INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Core/Include)

# STM32 CMSIS device.
add_library(stm32n6_cmsis INTERFACE)
target_include_directories(stm32n6_cmsis INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32N6xx/Include)
target_link_libraries(stm32n6_cmsis INTERFACE cmsis)

# Linker script of the application image; src/application reads its path from
# this property.
set_target_properties(system PROPERTIES STM32N6_LINKER_SCRIPT
    ${CMAKE_CURRENT_SOURCE_DIR}/linker/STM32N657XX_XIP.ld)
