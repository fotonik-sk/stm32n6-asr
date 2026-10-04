# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# Cortex-M55 / hard-float / CMSE, ST Edge AI Core 4.0, LL_ATON 1.1.3-262.
# The public header checks the runtime ABI; weights are flashed separately.
add_library(asr_network STATIC IMPORTED GLOBAL)
set_target_properties(asr_network PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libcynipsnet.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
    INTERFACE_LINK_LIBRARIES stedgeai)
