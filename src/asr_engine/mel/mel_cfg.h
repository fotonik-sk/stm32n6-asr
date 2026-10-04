/**
 * @file     mel_cfg.h
 * @brief    Geometry the mel stage derives from the model parameters.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_MEL_MEL_CFG_H
#define ASR_ENGINE_MEL_MEL_CFG_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

/* Model parameters shared by every stage. */
#include "asr_model_params.h"

/* Host tunables (see asr_engine_conf_template.h). */
#include "asr_engine_conf.h"

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/** @brief Samples a frame shares with the previous one; carried between blocks. */
#define MEL_FRAME_OVERLAP_SAMPLES (ASR_MODEL_MEL_FRAME_SAMPLES - ASR_MODEL_MEL_HOP_SAMPLES)

/** @brief Non-redundant spectrum bins produced by the real FFT. */
#define MEL_SPECTRUM_BINS ((ASR_MODEL_MEL_FFT_SIZE / 2U) + 1U)

/**
 * @brief Pool for the non-zero mel filter weights; 498 are used now.
 * @note mel_tables_init() fails if a new configuration overflows it.
 */
#define MEL_FILTER_WEIGHT_CAPACITY 640U

#endif /* ASR_ENGINE_MEL_MEL_CFG_H */
