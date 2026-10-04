# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Microphone, plugged into asr_engine as its audio port.
add_library(app_audio audio_port.c)
target_include_directories(app_audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app_audio PUBLIC asr_engine)
target_link_libraries(app_audio PRIVATE bsp drv_pdm)
