# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Application layer: the firmware image and the modules it starts.
#
#   main.c         entry point: bsp_init(), then the ThreadX kernel
#   app_startup.c  the start-up table: which module starts when
#   config/        priority and stack of every thread; asr_engine settings
#   led/           heartbeat LED thread
#   display/       LCD: startup logo, then the transcript thread
#   audio/         microphone, plugged into asr_engine as its audio port
add_subdirectory(config)
add_subdirectory(led)
add_subdirectory(display)
add_subdirectory(audio)

# The application image: runs in place from external NOR at 0x70100400.
get_target_property(APP_LINKER_SCRIPT system STM32N6_LINKER_SCRIPT)

add_executable(app
    main.c
    app_startup.c)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_options(app PRIVATE
    -T${APP_LINKER_SCRIPT}
    -Wl,-Map=$<TARGET_FILE_DIR:app>/app.map,--cref)
target_link_libraries(app PRIVATE
    system
    stm32n6_hal
    stm32n6_hal_msp
    stm32n6_it
    bsp
    drv_uart
    drv_npu
    logger
    threadx
    asr_engine
    app_config
    app_led
    app_display
    app_audio)

stm32n6_track_linker_script(app ${APP_LINKER_SCRIPT})
stm32n6_emit_image(app)
