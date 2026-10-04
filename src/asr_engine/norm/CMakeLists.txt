# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Normalization stage: statistics and int8 quantization (norm.c) and the thread
# that feeds it from the mel ring.
add_library(asr_norm
    norm.c
    norm_thread.c)
# The parent directory provides asr_model_params.h.
target_include_directories(asr_norm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
# No FMA contraction: the statistics round every operation separately.
target_compile_options(asr_norm PRIVATE -ffp-contract=off)
target_link_libraries(asr_norm PUBLIC m)
target_link_libraries(asr_norm PUBLIC threadx)
target_link_libraries(asr_norm PUBLIC asr_engine_conf)
target_link_libraries(asr_norm PUBLIC asr_engine_network_shape)
target_link_libraries(asr_norm PRIVATE asr_mel logger)
