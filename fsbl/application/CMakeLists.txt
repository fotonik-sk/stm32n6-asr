# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# FSBL entry point. Links the system, HAL and External Memory Manager and is
# linked into AXISRAM2 by the FSBL linker script.
get_target_property(FSBL_LINKER_SCRIPT system STM32N6_LINKER_SCRIPT)

add_executable(fsbl
    main.c
    ${CMAKE_CURRENT_LIST_DIR}/../../src/hardware/mcu/clock/mcu_clock.c)
target_include_directories(fsbl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../src/hardware/mcu/clock)
target_link_options(fsbl PRIVATE
    -T${FSBL_LINKER_SCRIPT}
    -Wl,-Map=$<TARGET_FILE_DIR:fsbl>/fsbl.map,--cref)
target_link_libraries(fsbl PRIVATE
    system
    stm32n6_hal
    stm32n6_hal_msp
    fsbl_it
    extmem)

stm32n6_track_linker_script(fsbl ${FSBL_LINKER_SCRIPT})
stm32n6_emit_image(fsbl)
