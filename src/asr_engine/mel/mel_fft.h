/**
 * @file     mel_fft.h
 * @brief    Radix-2 float32 FFT producing the power spectrum of one frame.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_MEL_MEL_FFT_H
#define ASR_ENGINE_MEL_MEL_FFT_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mel_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/** @brief Build the twiddle table; idempotent. */
void mel_fft_init(void);

/**
 * @brief Return the power spectrum of one real frame.
 * @param[in] time_domain @c ASR_MODEL_MEL_FFT_SIZE windowed, zero-padded samples.
 * @param[out] power @c MEL_SPECTRUM_BINS power values, bin 0 first.
 * @pre mel_fft_init() has run.
 * @note Not reentrant.
 */
void mel_fft_power(const float *time_domain, float *power);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_MEL_MEL_FFT_H */
