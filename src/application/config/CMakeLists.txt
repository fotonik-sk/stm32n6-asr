# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Configuration headers owned by the application:
#   app_threads.h     priority and stack of every thread in the firmware
#   asr_engine_conf.h  this board's settings of the asr_engine layer
add_library(app_config INTERFACE)
target_include_directories(app_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# The target name asr_engine links its configuration by.
add_library(asr_engine_conf ALIAS app_config)
