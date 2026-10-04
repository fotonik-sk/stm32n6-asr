# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Drivers for external ICs (ST BSP "Components"), unmodified; the *_conf.h next
# to each is the project configuration. Used by the ST BSP XSPI driver.

# MX66UW1G45G octo-SPI NOR flash (XSPI2).
add_library(mx66uw1g45g mx66uw1g45g/mx66uw1g45g.c)
target_include_directories(mx66uw1g45g PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mx66uw1g45g)
target_link_libraries(mx66uw1g45g PUBLIC stm32n6_hal)

# APS256XX hexa-deca-SPI PSRAM (XSPI1).
add_library(aps256xx aps256xx/aps256xx.c)
target_include_directories(aps256xx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/aps256xx)
target_link_libraries(aps256xx PUBLIC stm32n6_hal)
