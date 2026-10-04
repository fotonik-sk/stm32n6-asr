/**
 * @file     norm_cfg.h
 * @brief    Buffer geometry of the normalization stage.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_NORM_NORM_CFG_H
#define ASR_ENGINE_NORM_NORM_CFG_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

/* Model parameters shared by every stage. */
#include "asr_model_params.h"

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/** @brief Blocks in one receptive field, which is also one network window. */
#define NORM_WINDOW_BLOCKS (ASR_NETWORK_INPUT_FRAMES / ASR_MODEL_NET_STRIDE_FRAMES)

/** @brief Blocks the statistics are kept for; one ring slot each. */
#define NORM_STATS_BLOCKS (ASR_MODEL_NORM_STATS_FRAMES / ASR_MODEL_NET_STRIDE_FRAMES)

/** @brief Values in one block. */
#define NORM_BLOCK_VALUES (ASR_MODEL_NET_STRIDE_FRAMES * ASR_MODEL_MEL_BANDS)

/** @brief Values in one receptive field, @c [frame][band]: float field and int8 input alike. */
#define NORM_FIELD_VALUES (ASR_NETWORK_INPUT_FRAMES * ASR_MODEL_MEL_BANDS)

#endif /* ASR_ENGINE_NORM_NORM_CFG_H */
