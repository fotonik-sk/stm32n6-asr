/**
 * @file     net_cfg.h
 * @brief    Window geometry of the network stage.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_NET_NET_CFG_H
#define ASR_ENGINE_NET_NET_CFG_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

/* Model parameters shared by every stage. */
#include "asr_model_params.h"

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/** @brief Input frames per output step. */
#define NET_FRAMES_PER_STEP (ASR_NETWORK_INPUT_FRAMES / ASR_NETWORK_OUTPUT_STEPS)

/** @brief Output steps of the left context. */
#define NET_LEFT_CONTEXT_STEPS (ASR_MODEL_NET_LEFT_CONTEXT_FRAMES / NET_FRAMES_PER_STEP)

/** @brief Output steps of the core, the part of a window decoded as final text. */
#define NET_CORE_CONTEXT_STEPS (ASR_MODEL_NET_STRIDE_FRAMES / NET_FRAMES_PER_STEP)

/** @brief Output steps of the right context. */
#define NET_RIGHT_CONTEXT_STEPS (ASR_MODEL_NET_RIGHT_CONTEXT_FRAMES / NET_FRAMES_PER_STEP)

/** @brief Blank output steps that separate two words. */
#define NET_PAUSE_SPACE_STEPS (ASR_MODEL_NET_PAUSE_SPACE_FRAMES / NET_FRAMES_PER_STEP)

/** @brief Bytes of the int8 input tensor, @c [frame][band]. */
#define NET_INPUT_BYTES (ASR_NETWORK_INPUT_FRAMES * ASR_MODEL_MEL_BANDS)

/** @brief Bytes of the int8 output tensor, @c [step][class]. */
#define NET_OUTPUT_BYTES (ASR_NETWORK_OUTPUT_STEPS * ASR_NETWORK_OUTPUT_CLASSES)

/** @brief D-cache line of the Cortex-M55; cache maintenance works in whole lines. */
#define NET_CACHE_LINE_BYTES 32U

/** @brief Offset of the scratch area after the input tensor, rounded up to a cache line. */
#define NET_SCRATCH_OFFSET \
    (((NET_INPUT_BYTES + NET_CACHE_LINE_BYTES) - 1U) / NET_CACHE_LINE_BYTES * NET_CACHE_LINE_BYTES)

/** @brief Bytes of the input pool after the input tensor, free until the first inference. */
#define NET_SCRATCH_BYTES (ASR_NETWORK_INPUT_POOL_BYTES - NET_SCRATCH_OFFSET)

/**
 * @brief Characters one decode can produce, terminator included.
 * @note One per step, plus a space after blanks of the previous decode.
 */
#define NET_TEXT_CAPACITY (ASR_NETWORK_OUTPUT_STEPS + 2U)

#endif /* ASR_ENGINE_NET_NET_CFG_H */
