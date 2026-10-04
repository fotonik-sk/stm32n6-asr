# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Network stage: NPU wrapper (net_npu.c), greedy CTC decoder (net_ctc.c) and the
# inference thread (net_thread.c).
add_library(asr_net
    net_ctc.c
    net_npu.c
    net_thread.c)
# The parent directory provides asr_model_params.h and asr_engine.h.
target_include_directories(asr_net PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(asr_net PUBLIC threadx)
target_link_libraries(asr_net PUBLIC asr_engine_conf)
target_link_libraries(asr_net PUBLIC asr_engine_network_shape)
# asr_network is the generated network selected in ../CMakeLists.txt.
target_link_libraries(asr_net PRIVATE asr_network stedgeai logger)
