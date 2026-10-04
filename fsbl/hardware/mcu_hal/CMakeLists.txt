# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# HAL configuration header (stm32n6xx_hal_conf.h) + CubeMX main.h.
add_library(stm32n6_hal_conf INTERFACE)
target_include_directories(stm32n6_hal_conf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Vendor HAL (in-tree). Same module-gating as the app: compile only
# stm32n6xx_hal*.c, exclude *_template.c and the LL units.
set(HAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/STM32N6xx_HAL_Driver)
file(GLOB HAL_SOURCES CONFIGURE_DEPENDS ${HAL_DIR}/Src/stm32n6xx_hal*.c)
list(FILTER HAL_SOURCES EXCLUDE REGEX ".*_template\\.c$")

add_library(stm32n6_hal OBJECT ${HAL_SOURCES})
target_include_directories(stm32n6_hal PUBLIC ${HAL_DIR}/Inc)
target_compile_definitions(stm32n6_hal PUBLIC USE_HAL_DRIVER)
target_link_libraries(stm32n6_hal PUBLIC stm32n6_hal_conf stm32n6_cmsis)
# Upstream code stays unmodified, so its warnings are not ours to fix.
target_compile_options(stm32n6_hal PRIVATE -w)

# HAL MSP.
# OBJECT (not a static archive): the MSP provides STRONG overrides of the HAL's
# __weak HAL_MspInit / HAL_XSPI_MspInit. The weak stubs are already pulled in
# unconditionally by the stm32n6_hal OBJECT lib, so a static-archive MSP would
# never be extracted (no undefined ref to resolve) and the empty weak stubs would
# win -- leaving XSPI2 unclocked and hanging HAL_XSPI_Init. OBJECT forces the
# strong definitions into the link.
add_library(stm32n6_hal_msp OBJECT stm32n6xx_hal_msp.c)
target_include_directories(stm32n6_hal_msp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(stm32n6_hal_msp PUBLIC stm32n6_hal)
