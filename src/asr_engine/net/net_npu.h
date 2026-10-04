/**
 * @file     net_npu.h
 * @brief    The network on the NPU: runtime start-up, tensors, one inference.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_NET_NET_NPU_H
#define ASR_ENGINE_NET_NET_NPU_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "net_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Result of an NPU operation. */
typedef enum
{
    NET_NPU_OK = 0, /**< The operation completed. */
    NET_NPU_ERROR   /**< The runtime reported an error, or the unit is not initialized. */
} net_npu_status_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Start the NPU runtime and bind the generated network.
 * @retval NET_NPU_OK The runtime runs and the tensor addresses are known.
 * @retval NET_NPU_ERROR Start-up failed, or the input is not cache-line aligned.
 * @pre The NPU and its memories are up (asr_engine.h). Call from a thread.
 */
net_npu_status_t net_npu_init(void);

/**
 * @brief Return the input tensor.
 * @return @c NET_INPUT_BYTES int8 codes, @c [frame][band]; NULL before net_npu_init().
 * @note Inference overwrites it: write a complete window before every run.
 */
int8_t *net_npu_input(void);

/**
 * @brief Return the output tensor of the last inference; valid until the next run.
 * @return @c NET_OUTPUT_BYTES int8 logits, @c [step][class]; NULL before net_npu_init().
 */
const int8_t *net_npu_output(void);

/**
 * @brief Return the rest of the input pool after the tensor; free until the first run.
 * @return @c NET_SCRATCH_BYTES cache-line-aligned bytes; NULL before net_npu_init().
 */
void *net_npu_scratch(void);

/**
 * @brief Run one inference over the input tensor and wait for it.
 * @param[in] discard_scratch Non-zero on the first run: invalidate, without
 *            write-back, the cache lines scratch users left.
 * @retval NET_NPU_OK The output tensor holds the result.
 * @retval NET_NPU_ERROR The runtime reported an error, or the unit is not initialized.
 */
net_npu_status_t net_npu_run(int discard_scratch);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_NET_NET_NPU_H */
