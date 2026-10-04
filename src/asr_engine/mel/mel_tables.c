/**
 * @file     mel_tables.c
 * @brief    Runtime construction of the Hann window and the Slaney mel bank.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mel_tables.h"

#include <math.h>
#include <stddef.h>

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief Pi in double precision; M_PI is not part of C11. */
#define MEL_PI 3.14159265358979323846

/** @brief Hertz per mel below the logarithmic knee of the Slaney scale. */
#define MEL_SLANEY_F_SP (200.0 / 3.0)

/** @brief Frequency where the Slaney scale switches from linear to logarithmic. */
#define MEL_SLANEY_MIN_LOG_HZ 1000.0

/** @brief Mel position of MEL_SLANEY_MIN_LOG_HZ, where the logarithmic part starts (15). */
#define MEL_SLANEY_MIN_LOG_MEL (MEL_SLANEY_MIN_LOG_HZ / MEL_SLANEY_F_SP)

/** @brief Natural log of the frequency ratio per mel above the knee: 27 mels span x6.4. */
#define MEL_SLANEY_LOG_STEP (log(6.4) / 27.0)

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Symmetric Hann window applied to every frame. */
static float window[ASR_MODEL_MEL_FRAME_SAMPLES];

/** @brief Bin span and weight offset of every mel band. */
static mel_filter_span_t filters[ASR_MODEL_MEL_BANDS];

/** @brief Non-zero triangular weights of all bands, packed back to back. */
static float weights[MEL_FILTER_WEIGHT_CAPACITY];

/** @brief Entries of weights that the built bank occupies. */
static uint32_t weight_count;

/** @brief Set once the tables hold valid data. */
static int ready;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Convert hertz to the Slaney mel scale.
 * @param[in] hz Frequency in hertz.
 * @return Position on the mel scale.
 * @note Linear below 1 kHz, logarithmic above it.
 */
static double hz_to_mel(double hz)
{
    if (hz < MEL_SLANEY_MIN_LOG_HZ)
    {
        return hz / MEL_SLANEY_F_SP;
    }
    return MEL_SLANEY_MIN_LOG_MEL + (log(hz / MEL_SLANEY_MIN_LOG_HZ) / MEL_SLANEY_LOG_STEP);
}

/**
 * @brief Convert a Slaney mel position back to hertz.
 * @param[in] mel Position on the mel scale.
 * @return Frequency in hertz.
 */
static double mel_to_hz(double mel)
{
    if (mel < MEL_SLANEY_MIN_LOG_MEL)
    {
        return mel * MEL_SLANEY_F_SP;
    }
    return MEL_SLANEY_MIN_LOG_HZ * exp(MEL_SLANEY_LOG_STEP * (mel - MEL_SLANEY_MIN_LOG_MEL));
}

/**
 * @brief Fill window with the symmetric Hann window of the frame length.
 */
static void build_window(void)
{
    uint32_t n;

    for (n = 0U; n < ASR_MODEL_MEL_FRAME_SAMPLES; ++n)
    {
        double phase = (2.0 * MEL_PI * (double)n) / (double)(ASR_MODEL_MEL_FRAME_SAMPLES - 1U);

        window[n] = (float)(0.5 - (0.5 * cos(phase)));
    }
}

/**
 * @brief Weight of one triangular filter at one spectrum bin.
 * @param[in] bin_hz Centre frequency of the bin.
 * @param[in] left Lower edge of the triangle in hertz.
 * @param[in] centre Peak of the triangle in hertz.
 * @param[in] right Upper edge of the triangle in hertz.
 * @return Slaney area-normalized weight; 0 outside the triangle.
 */
static double triangle_weight(double bin_hz, double left, double centre, double right)
{
    double rising = (bin_hz - left) / (centre - left);
    double falling = (right - bin_hz) / (right - centre);
    double weight = (rising < falling) ? rising : falling;

    if (weight <= 0.0)
    {
        return 0.0;
    }
    /* Slaney area normalization: the triangle integrates to a constant. */
    return (weight * 2.0) / (right - left);
}

/**
 * @brief Fill filters and weights with the sparse mel filter bank.
 * @retval 0 The bank fits the weight pool.
 * @retval -1 The bank needs more than MEL_FILTER_WEIGHT_CAPACITY weights.
 */
static int build_filters(void)
{
    double edges[ASR_MODEL_MEL_BANDS + 2U];
    double mel_low = hz_to_mel(ASR_MODEL_MEL_LOWER_EDGE_HZ);
    double mel_high = hz_to_mel(ASR_MODEL_MEL_UPPER_EDGE_HZ);
    double bin_hz_step = ASR_MODEL_MEL_UPPER_EDGE_HZ / (double)(ASR_MODEL_MEL_FFT_SIZE / 2U);
    uint32_t index;
    uint32_t band;

    /* ASR_MODEL_MEL_BANDS + 2 band edges, evenly spaced on the mel scale. */
    for (index = 0U; index < (ASR_MODEL_MEL_BANDS + 2U); ++index)
    {
        double position =
            mel_low + (((mel_high - mel_low) * (double)index) / (double)(ASR_MODEL_MEL_BANDS + 1U));

        edges[index] = mel_to_hz(position);
    }

    weight_count = 0U;
    for (band = 0U; band < ASR_MODEL_MEL_BANDS; ++band)
    {
        double left = edges[band];
        double centre = edges[band + 1U];
        double right = edges[band + 2U];
        uint32_t first = 0U;
        uint32_t last = 0U;
        int found = 0;
        uint32_t bin;

        for (bin = 0U; bin < MEL_SPECTRUM_BINS; ++bin)
        {
            if (triangle_weight((double)bin * bin_hz_step, left, centre, right) > 0.0)
            {
                if (!found)
                {
                    first = bin;
                    found = 1;
                }
                last = bin;
            }
        }

        filters[band].start = (uint16_t)first;
        filters[band].count = found ? (uint16_t)((last - first) + 1U) : 0U;
        filters[band].offset = (uint16_t)weight_count;

        if ((weight_count + filters[band].count) > MEL_FILTER_WEIGHT_CAPACITY)
        {
            weight_count = 0U;
            return -1;
        }
        for (bin = first; found && (bin <= last); ++bin)
        {
            weights[weight_count] =
                (float)triangle_weight((double)bin * bin_hz_step, left, centre, right);
            ++weight_count;
        }
    }
    return 0;
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

int mel_tables_init(void)
{
    if (ready)
    {
        return 0;
    }
    build_window();
    if (build_filters() != 0)
    {
        return -1;
    }
    ready = 1;
    return 0;
}

const float *mel_tables_window(void)
{
    return ready ? window : NULL;
}

const mel_filter_span_t *mel_tables_filters(void)
{
    return ready ? filters : NULL;
}

const float *mel_tables_weights(void)
{
    return ready ? weights : NULL;
}

uint32_t mel_tables_weight_count(void)
{
    return ready ? weight_count : 0U;
}
