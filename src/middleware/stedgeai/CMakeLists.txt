# SPDX-FileCopyrightText: 2026 Nikita Prismakov
# SPDX-License-Identifier: Apache-2.0

# ST Edge AI NPU runtime: LL_ATON (epoch-block scheduler, NPU and cache
# drivers, RTOS glue) and the ST.AI API the generated networks are called
# through. Vendored unmodified from Middlewares/ST/AI of ST Edge AI Core 4.0
# (LL_ATON 1.1.3), the release the networks under src/asr_engine/models/ were
# compiled with; licence terms in LICENSE.html. ST publishes it only inside
# that package, so it is kept in-tree like the HAL rather than as a submodule.
#
# Only the units the ThreadX, asynchronous, non-relocatable configuration below
# needs are compiled; the rest of Npu/ll_aton stays for reference.
set(STEDGEAI_LL_ATON ${CMAKE_CURRENT_SOURCE_DIR}/Npu/ll_aton)
set(STEDGEAI_DEVICE ${CMAKE_CURRENT_SOURCE_DIR}/Npu/Devices/STM32N6xx)

add_library(stedgeai
    ${STEDGEAI_LL_ATON}/ecloader.c
    ${STEDGEAI_LL_ATON}/ll_aton.c
    ${STEDGEAI_LL_ATON}/ll_aton_cipher.c
    ${STEDGEAI_LL_ATON}/ll_aton_dbgtrc.c
    ${STEDGEAI_LL_ATON}/ll_aton_debug.c
    ${STEDGEAI_LL_ATON}/ll_aton_lib.c
    ${STEDGEAI_LL_ATON}/ll_aton_lib_sw_operators.c
    ${STEDGEAI_LL_ATON}/ll_aton_osal_threadx.c
    ${STEDGEAI_LL_ATON}/ll_aton_runtime.c
    ${STEDGEAI_LL_ATON}/ll_aton_stai_internal.c
    ${STEDGEAI_LL_ATON}/ll_aton_util.c
    ${STEDGEAI_DEVICE}/mcu_cache.c
    ${STEDGEAI_DEVICE}/npu_cache.c)
target_include_directories(stedgeai PUBLIC
    ${STEDGEAI_LL_ATON}
    ${STEDGEAI_DEVICE}
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
# The runtime and every generated network must be compiled with the same
# LL_ATON configuration (stai_network.h insists), hence PUBLIC:
#   - ThreadX OSAL: an inference sleeps on a semaphore the NPU interrupt
#     (NPU0_IRQHandler, defined by ll_aton_runtime.c) releases;
#   - one network at a time, so no cross-thread arbitration of the NPU;
#   - no per-buffer debug tables in the generated code.
target_compile_definitions(stedgeai PUBLIC
    # NDEBUG changes NN_Instance_TypeDef: keep the runtime and all consumers
    # ABI-compatible with the prebuilt release network, even in Debug builds.
    NDEBUG
    LL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6
    LL_ATON_OSAL=LL_ATON_OSAL_THREADX
    LL_ATON_RT_MODE=LL_ATON_RT_ASYNC
    LL_ATON_DBG_BUFFER_INFO_EXCLUDED=1
    APP_HAS_PARALLEL_NETWORKS=0)
# npu_cache.c drives CACHEAXI through the HAL (HAL_CACHEAXI_MODULE_ENABLED).
target_link_libraries(stedgeai PUBLIC threadx stm32n6_hal)
# Vendor code: optimized whatever the project's level (its interrupt handler
# and epoch scheduling run for every inference), and its warnings are ST's.
target_compile_options(stedgeai PRIVATE -O2 -w)
