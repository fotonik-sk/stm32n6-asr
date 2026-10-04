/**
 * @file     mel_tables.h
 * @brief    Hann window and sparse Slaney mel filter bank, built at runtime.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_MEL_MEL_TABLES_H
#define ASR_ENGINE_MEL_MEL_TABLES_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "mel_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Contiguous non-zero weights of one mel band, as a span of the shared weight pool. */
typedef struct
{
    uint16_t start;  /**< First spectrum bin the band covers. */
    uint16_t count;  /**< Bins the band covers; 0 for an empty band. */
    uint16_t offset; /**< Index of the band's first weight in the weight pool. */
} mel_filter_span_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Build the Hann window and the mel filter bank.
 * @retval 0 The tables are ready; a repeated call does not rebuild them.
 * @retval -1 The bank needs more than @c MEL_FILTER_WEIGHT_CAPACITY weights.
 */
int mel_tables_init(void);

/**
 * @brief Return the symmetric Hann window (divisor @c L-1).
 * @return @c ASR_MODEL_MEL_FRAME_SAMPLES coefficients, or NULL before initialization.
 */
const float *mel_tables_window(void);

/**
 * @brief Return the per-band bin spans of the filter bank.
 * @return @c ASR_MODEL_MEL_BANDS spans, or NULL before initialization.
 */
const mel_filter_span_t *mel_tables_filters(void);

/**
 * @brief Return the pool the spans index into.
 * @return Filter weights, or NULL before initialization.
 */
const float *mel_tables_weights(void);

/**
 * @brief Return how many weights the bank actually uses.
 * @return Used entries of the weight pool, or 0 before initialization.
 */
uint32_t mel_tables_weight_count(void);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_MEL_MEL_TABLES_H */
