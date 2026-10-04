# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# STM32 External Memory Manager (in-tree), configured by stm32_extmem_conf.h for
# the MX66UW1G45G NOR on XSPI2. Boot mode is execute-in-place (XIP): the FSBL
# maps the NOR flash and jumps to the application in place.
set(EXTMEM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/STM32_ExtMem_Manager)

add_library(extmem
    ${EXTMEM_DIR}/stm32_extmem.c
    ${EXTMEM_DIR}/boot/stm32_boot_xip.c
    ${EXTMEM_DIR}/nor_sfdp/stm32_sfdp_driver.c
    ${EXTMEM_DIR}/nor_sfdp/stm32_sfdp_data.c
    ${EXTMEM_DIR}/sal/stm32_sal_xspi.c)

target_include_directories(extmem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${EXTMEM_DIR}
    ${EXTMEM_DIR}/boot
    ${EXTMEM_DIR}/nor_sfdp
    ${EXTMEM_DIR}/sal)

target_link_libraries(extmem PUBLIC stm32n6_hal)
