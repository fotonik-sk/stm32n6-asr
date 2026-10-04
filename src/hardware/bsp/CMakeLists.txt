# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Board support package for STM32N6570-DK: board description, ST BSP, the
# bsp_* modules that wire drivers to this board, and the interrupt vectors.

# Board headers: boards.h + stm32n6570dk.h (the board description, also the ST
# BSP configuration) + the ST BSP headers. Interface-only so any hardware target
# can resolve them without a link dependency.
add_library(boards INTERFACE)
target_include_directories(boards INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/STM32N6570-DK)
target_link_libraries(boards INTERFACE stm32n6_hal)

# ST board support driver (BSP_LED_*, BSP_XSPI_RAM_*, ...), unmodified.
add_library(bsp_st OBJECT
    STM32N6570-DK/stm32n6570_discovery.c
    STM32N6570-DK/stm32n6570_discovery_xspi.c)
target_link_libraries(bsp_st PUBLIC boards mx66uw1g45g aps256xx)

# Project BSP: start-up sequence and board devices.
add_library(bsp
    bsp.c
    bsp_console.c
    bsp_lcd.c
    bsp_led.c
    bsp_memory.c
    bsp_mic.c)
target_include_directories(bsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bsp
    PUBLIC boards stm32n6_hal drv_pdm
    PRIVATE bsp_st drv_uart drv_ltdc mcu_core)

# Interrupt vector handlers (one file for the whole image): the board decides
# which peripherals are in use and routes their vectors to the drivers.
add_library(stm32n6_it OBJECT stm32n6xx_it.c)
target_include_directories(stm32n6_it PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(stm32n6_it PRIVATE stm32n6_hal drv_uart drv_pdm)
