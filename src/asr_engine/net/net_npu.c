/**
 * @file     net_npu.c
 * @brief    The network on the NPU; implements net_npu.h over the ST.AI API.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "net_npu.h"

#include <stddef.h>

#include "ll_aton.h"
#include "ll_aton_caches_interface.h"
#include "npu_cache.h"
#include "stai_network.h"

/*******************************************************************************/
/*                              STATIC ASSERTIONS                              */
/*******************************************************************************/

/* The generated network must be the one asr_model_params.h describes. */
_Static_assert((STAI_NETWORK_IN_NUM == 1) && (STAI_NETWORK_OUT_NUM == 1),
               "the stage drives a network with one input and one output");
_Static_assert((STAI_NETWORK_IN_1_WIDTH == ASR_NETWORK_INPUT_FRAMES) &&
               (STAI_NETWORK_IN_1_CHANNEL == ASR_MODEL_MEL_BANDS),
               "the generated network does not take ASR_NETWORK_INPUT_FRAMES x ASR_MODEL_MEL_BANDS");
_Static_assert(STAI_NETWORK_IN_1_SIZE_BYTES == NET_INPUT_BYTES,
               "the input tensor of the generated network is not int8");
_Static_assert((STAI_NETWORK_OUT_1_WIDTH == ASR_NETWORK_OUTPUT_STEPS) &&
               (STAI_NETWORK_OUT_1_CHANNEL == ASR_NETWORK_OUTPUT_CLASSES),
               "the generated network does not output ASR_NETWORK_OUTPUT_STEPS x ASR_NETWORK_OUTPUT_CLASSES");
_Static_assert(STAI_NETWORK_OUT_1_SIZE_BYTES == NET_OUTPUT_BYTES,
               "the output tensor of the generated network is not int8");
_Static_assert(ASR_NETWORK_INPUT_POOL_BYTES > NET_SCRATCH_OFFSET,
               "the input tensor does not fit ASR_NETWORK_INPUT_POOL_BYTES");

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Context of the network instance, sized by the generated header. */
static STAI_NETWORK_CONTEXT_DECLARE(network, STAI_NETWORK_CONTEXT_SIZE)

/** @brief Input tensor inside the activation pools. */
static int8_t *input;

/** @brief Output tensor inside the activation pools. */
static const int8_t *output;

/** @brief Set once net_npu_init() has succeeded. */
static int ready;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

net_npu_status_t net_npu_init(void)
{
    stai_ptr inputs[STAI_NETWORK_IN_NUM];
    stai_ptr outputs[STAI_NETWORK_OUT_NUM];
    stai_size count = 0U;

    if (ready)
    {
        return NET_NPU_OK;
    }

    /* The generated code maintains the NPU cache around external pools. */
    npu_cache_enable();

    if ((stai_runtime_init() != STAI_SUCCESS) || (stai_network_init(network) != STAI_SUCCESS))
    {
        return NET_NPU_ERROR;
    }
    if ((stai_network_get_inputs(network, inputs, &count) != STAI_SUCCESS) ||
        (count != STAI_NETWORK_IN_NUM))
    {
        return NET_NPU_ERROR;
    }
    if ((stai_network_get_outputs(network, outputs, &count) != STAI_SUCCESS) ||
        (count != STAI_NETWORK_OUT_NUM))
    {
        return NET_NPU_ERROR;
    }
    input = (int8_t *)inputs[0];
    output = (const int8_t *)outputs[0];

    /* Cache maintenance of the input, and the scratch after it, work in lines. */
    if ((input == NULL) || (output == NULL) ||
        (((uintptr_t)input % NET_CACHE_LINE_BYTES) != 0U))
    {
        return NET_NPU_ERROR;
    }

    ready = 1;
    return NET_NPU_OK;
}

int8_t *net_npu_input(void)
{
    return ready ? input : NULL;
}

const int8_t *net_npu_output(void)
{
    return ready ? output : NULL;
}

void *net_npu_scratch(void)
{
    return ready ? (void *)&input[NET_SCRATCH_OFFSET] : NULL;
}

net_npu_status_t net_npu_run(int discard_scratch)
{
    if (!ready)
    {
        return NET_NPU_ERROR;
    }

    /* The window was written through the D-cache, the NPU reads memory. */
    LL_ATON_Cache_MCU_Clean_Invalidate_Range((uintptr_t)input, NET_INPUT_BYTES);
    if (discard_scratch)
    {
        /* Otherwise dirty scratch lines may be written back over activations. */
        LL_ATON_Cache_MCU_Invalidate_Range((uintptr_t)&input[NET_SCRATCH_OFFSET], NET_SCRATCH_BYTES);
    }

    /* Sleeps on the runtime semaphore between epochs; the generated code
       invalidates the output cache lines itself. */
    if (stai_network_run(network, STAI_MODE_SYNC) != STAI_SUCCESS)
    {
        return NET_NPU_ERROR;
    }
    return NET_NPU_OK;
}
