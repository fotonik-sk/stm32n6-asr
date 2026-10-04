# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# HAL configuration header (stm32n6xx_hal_conf.h).
add_library(stm32n6_hal_conf INTERFACE)
target_include_directories(stm32n6_hal_conf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# (stm32n6xx_hal*.c); the LL drivers are pulled in through inline headers only,
# so their .c units are excluded (they reference unconfigured types). Unused
# peripherals are gated off by stm32n6xx_hal_conf.h and dropped by --gc-sections.
set(HAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/STM32N6xx_HAL_Driver)
file(GLOB HAL_SOURCES CONFIGURE_DEPENDS ${HAL_DIR}/Src/stm32n6xx_hal*.c)
list(FILTER HAL_SOURCES EXCLUDE REGEX ".*_template\\.c$")

add_library(stm32n6_hal OBJECT ${HAL_SOURCES})
target_include_directories(stm32n6_hal PUBLIC ${HAL_DIR}/Inc)
target_compile_definitions(stm32n6_hal PUBLIC USE_HAL_DRIVER)
target_link_libraries(stm32n6_hal PUBLIC stm32n6_hal_conf stm32n6_cmsis)
# Upstream code stays unmodified, so its warnings are not ours to fix.
target_compile_options(stm32n6_hal PRIVATE -w)

# HAL MSP (peripheral GPIO/clock hooks).
add_library(stm32n6_hal_msp stm32n6xx_hal_msp.c)
target_include_directories(stm32n6_hal_msp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(stm32n6_hal_msp PUBLIC stm32n6_hal)
