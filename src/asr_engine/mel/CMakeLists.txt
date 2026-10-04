# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Mel stage: streaming converter, tables, FFT, feature ring and their thread.
# Only mel_thread.c needs ThreadX and the logger; mel_ring.c needs CMSIS-Core.
add_library(asr_mel
    mel.c
    mel_fft.c
    mel_tables.c
    mel_ring.c
    mel_thread.c)
# The parent directory provides asr_model_params.h and asr_engine.h.
target_include_directories(asr_mel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
# libm is PUBLIC so it links ahead of libc, which resolves libm's __errno.
target_link_libraries(asr_mel PUBLIC m)
target_link_libraries(asr_mel PUBLIC threadx)
target_link_libraries(asr_mel PUBLIC asr_engine_conf)
target_link_libraries(asr_mel PUBLIC asr_engine_network_shape)
target_link_libraries(asr_mel PRIVATE cmsis logger)
