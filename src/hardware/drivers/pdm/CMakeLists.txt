# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# PDM microphone capture: MDF1 configuration, capture DMA and chunked ring buffer.
add_subdirectory(ring)
add_subdirectory(dma)
add_subdirectory(mdf)

# drv_pdm.h is the only public header: the sub-modules are linked privately so that
# their headers and types do not leak into callers.
add_library(drv_pdm drv_pdm.c)
target_include_directories(drv_pdm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(drv_pdm PRIVATE
    drv_pdm_mdf
    drv_pdm_dma
    drv_pdm_ring)
