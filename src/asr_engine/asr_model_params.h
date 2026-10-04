/**
 * @file     asr_model_params.h
 * @brief    Parameters of the deployed ASR model, shared by every stage.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_ASR_MODEL_PARAMS_H
#define ASR_ENGINE_ASR_MODEL_PARAMS_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

/* Geometry and quantization of the compiled network (models/<network>/). */
#include "network_shape.h"

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/* Model identity. */

/** @brief Name of the deployed network, as reported in the log. */
#define ASR_MODEL_NAME "CynipsNet"

/** @brief Version of the deployed network, as reported in the log. */
#define ASR_MODEL_VERSION "1.0"

/* Log-mel front-end. */

/** @brief Input sample rate in hertz; the capture chain delivers mono at this rate. */
#define ASR_MODEL_SAMPLE_RATE_HZ 16000U

/** @brief Analysis frame length in samples (20 ms at 16 kHz). */
#define ASR_MODEL_MEL_FRAME_SAMPLES 320U

/** @brief Frame advance in samples (10 ms at 16 kHz); one feature frame per hop. */
#define ASR_MODEL_MEL_HOP_SAMPLES 160U

/** @brief Real FFT length; the windowed frame is zero-padded up to it. */
#define ASR_MODEL_MEL_FFT_SIZE 512U

/** @brief Mel bands per frame: the feature length of mel, norm and network input alike. */
#define ASR_MODEL_MEL_BANDS 64U

/** @brief Lowest edge of the mel filter bank in hertz. */
#define ASR_MODEL_MEL_LOWER_EDGE_HZ 0.0

/** @brief Highest edge of the mel filter bank in hertz (Nyquist). */
#define ASR_MODEL_MEL_UPPER_EDGE_HZ 8000.0

/** @brief Pre-emphasis coefficient: @c y[n] = x[n] - k * x[n-1]. */
#define ASR_MODEL_MEL_PREEMPHASIS 0.97f

/** @brief Exponent applied to the magnitude spectrum (2.0 = power spectrum). */
#define ASR_MODEL_MEL_SPECTRUM_POWER 2.0f

/** @brief Additive guard inside the logarithm (2^-24); silence maps to about -16.64. */
#define ASR_MODEL_MEL_LOG_ZERO_GUARD 5.9604644775390625e-08f

/* Normalization (receptive field): each network window is normalized per band
   with the statistics of the last ASR_MODEL_NORM_STATS_FRAMES, recomputed once
   per stride. */

/**
 * @brief Frames the mean and the deviation are taken over (30 s); tunable.
 * @note Costs 16 bytes per band and stride, not raw frames (60 KiB here).
 *       Longer is steadier; shorter follows a new speaker or room faster.
 */
#define ASR_MODEL_NORM_STATS_FRAMES 3000U

/** @brief Lower limit of the per-band deviation, so a constant band cannot divide by zero. */
#define ASR_MODEL_NORM_SIGMA_FLOOR 1e-6

/* Network. A window of ASR_NETWORK_INPUT_FRAMES, oldest first:
   | left context | core (= stride) | right context |
   Only the core becomes final text and windows advance by one core, so they
   tile the transcript. */

/**
 * @brief Oldest frames of a window; their output lacks the past and is
 *        dropped, except in the first window.
 */
#define ASR_MODEL_NET_LEFT_CONTEXT_FRAMES (370U + 50U)

/** @brief Window step (0.5 s): the core committed as final text, and the normalization block. */
#define ASR_MODEL_NET_STRIDE_FRAMES 50U

/** @brief Newest frames of a window; their output lacks the future and is only tentative. */
#define ASR_MODEL_NET_RIGHT_CONTEXT_FRAMES 130U

/** @brief Class of the CTC blank. */
#define ASR_MODEL_NET_BLANK_INDEX 0U

/** @brief Character of every output class, in class order; the blank's is a placeholder. */
#define ASR_MODEL_NET_TOKENS "_ ABCDEFGHIJKLMNOPQRSTUVWXYZ'"

/** @brief Class of the space, the word separator in @c ASR_MODEL_NET_TOKENS. */
#define ASR_MODEL_NET_SPACE_INDEX 1U

/**
 * @brief Silence (0.3 s) after which the decoder inserts a missed space; tunable.
 * @note Shorter joins fewer words but may split a slowly spoken one.
 */
#define ASR_MODEL_NET_PAUSE_SPACE_FRAMES 30U

#endif /* ASR_ENGINE_ASR_MODEL_PARAMS_H */
