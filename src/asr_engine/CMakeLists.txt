# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# ASR model layer: speech recognition as a replaceable block (see README.md).
# The host integrates it through asr_engine.h and supplies asr_engine_conf.h via
# the asr_engine_conf target.

# Compiled network under models/<name>/; it defines the target asr_network.
set(ASR_MODEL_NETWORK "cynipsnet_v1.0" CACHE STRING "Generated network under src/asr_engine/models/")

# models/<name>/network_shape.h, included by asr_model_params.h.
add_library(asr_engine_network_shape INTERFACE)
target_include_directories(asr_engine_network_shape INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/models/${ASR_MODEL_NETWORK})

add_subdirectory(mel)
add_subdirectory(norm)
add_subdirectory(models/${ASR_MODEL_NETWORK})
add_subdirectory(net)

# The layer itself only wires its stages together.
add_library(asr_engine asr_engine.c)
target_include_directories(asr_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(asr_engine PUBLIC asr_mel asr_norm asr_net)
