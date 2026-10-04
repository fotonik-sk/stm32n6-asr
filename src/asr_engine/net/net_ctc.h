/**
 * @file     net_ctc.h
 * @brief    Streaming greedy CTC decoding of the network output.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_NET_NET_CTC_H
#define ASR_ENGINE_NET_NET_CTC_H

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

/** @brief Decoder state carried between calls. */
typedef struct
{
    /** @brief Class of the last decoded step; -1 before the first one. */
    int32_t previous_class;
    /** @brief Class of the last character decoded; -1 before the first one. */
    int32_t printed_class;
    /** @brief Blank steps since the last non-blank one, saturated at @c NET_PAUSE_SPACE_STEPS. */
    uint32_t pause_steps;
} net_ctc_state_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Start a new stream.
 * @param[out] state Decoder state to clear.
 */
void net_ctc_reset(net_ctc_state_t *state);

/**
 * @brief Decode consecutive output steps, continuing the stream.
 * @param[in,out] state Decoder state; advanced over every step, even if @p text fills up.
 * @param[in] logits Network output, @c ASR_NETWORK_OUTPUT_STEPS x @c ASR_NETWORK_OUTPUT_CLASSES int8.
 * @param[in] first_step First step to decode.
 * @param[in] steps Steps to decode; the range must lie inside the output.
 * @param[out] text Receives the characters, NUL-terminated.
 * @param[in] capacity Size of @p text in bytes, terminator included.
 * @return Characters written without the terminator; 0 on an invalid argument.
 */
uint32_t net_ctc_decode(net_ctc_state_t *state,
                        const int8_t *logits,
                        uint32_t first_step,
                        uint32_t steps,
                        char *text,
                        uint32_t capacity);

/**
 * @brief Decode one window into its final and its tentative text.
 * @param[in,out] state Decoder state; advanced over the final part only.
 * @param[in] logits Network output of the window.
 * @param[in] first_window Non-zero for the first window, which also commits its left context.
 * @param[out] committed Text of the core, the final part.
 * @param[in] committed_capacity Size of @p committed in bytes.
 * @param[out] tentative Text of the right context, continuing @p committed.
 * @param[in] tentative_capacity Size of @p tentative in bytes.
 * @note @c NET_TEXT_CAPACITY bytes always suffice for either buffer.
 */
void net_ctc_decode_window(net_ctc_state_t *state,
                           const int8_t *logits,
                           int first_window,
                           char *committed,
                           uint32_t committed_capacity,
                           char *tentative,
                           uint32_t tentative_capacity);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_NET_NET_CTC_H */
