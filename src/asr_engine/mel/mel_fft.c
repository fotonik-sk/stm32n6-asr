/**
 * @file     mel_fft.c
 * @brief    In-place iterative radix-2 FFT over a real, zero-padded frame.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mel_fft.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief Pi in double precision; M_PI is not part of C11. */
#define MEL_PI 3.14159265358979323846

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Real part of the @c ASR_MODEL_MEL_FFT_SIZE/2 twiddle factors. */
static float twiddle_re[ASR_MODEL_MEL_FFT_SIZE / 2U];

/** @brief Imaginary part of the @c ASR_MODEL_MEL_FFT_SIZE/2 twiddle factors. */
static float twiddle_im[ASR_MODEL_MEL_FFT_SIZE / 2U];

/** @brief Real part of the transform in progress. */
static float real[ASR_MODEL_MEL_FFT_SIZE];

/** @brief Imaginary part of the transform in progress. */
static float imag[ASR_MODEL_MEL_FFT_SIZE];

/** @brief Number of butterfly stages, i.e. log2(ASR_MODEL_MEL_FFT_SIZE). */
static uint32_t stages;

/** @brief Set once the twiddle table holds valid data. */
static int ready;

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Reverse the low @p bits bits of an index.
 * @param[in] value Index to reverse.
 * @param[in] bits Width of the index in bits.
 * @return The bit-reversed index, used to order the decimation-in-time input.
 */
static uint32_t reverse_bits(uint32_t value, uint32_t bits)
{
    uint32_t result = 0U;
    uint32_t bit;

    for (bit = 0U; bit < bits; ++bit)
    {
        result = (result << 1U) | (value & 1U);
        value >>= 1U;
    }
    return result;
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void mel_fft_init(void)
{
    uint32_t index;
    uint32_t size;

    if (ready)
    {
        return;
    }
    for (index = 0U; index < (ASR_MODEL_MEL_FFT_SIZE / 2U); ++index)
    {
        double angle = (-2.0 * MEL_PI * (double)index) / (double)ASR_MODEL_MEL_FFT_SIZE;

        twiddle_re[index] = (float)cos(angle);
        twiddle_im[index] = (float)sin(angle);
    }
    stages = 0U;
    for (size = ASR_MODEL_MEL_FFT_SIZE; size > 1U; size >>= 1U)
    {
        ++stages;
    }
    ready = 1;
}

void mel_fft_power(const float *time_domain, float *power)
{
    uint32_t index;
    uint32_t length;
    uint32_t bin;

    if ((time_domain == NULL) || (power == NULL) || !ready)
    {
        return;
    }

    /* Decimation in time: bit-reversed load, butterflies in natural order. The
       input is real, so only the first MEL_SPECTRUM_BINS bins are read back. */
    for (index = 0U; index < ASR_MODEL_MEL_FFT_SIZE; ++index)
    {
        real[reverse_bits(index, stages)] = time_domain[index];
        imag[index] = 0.0f;
    }

    for (length = 2U; length <= ASR_MODEL_MEL_FFT_SIZE; length <<= 1U)
    {
        uint32_t half = length / 2U;
        uint32_t twiddle_step = ASR_MODEL_MEL_FFT_SIZE / length;
        uint32_t block;

        for (block = 0U; block < ASR_MODEL_MEL_FFT_SIZE; block += length)
        {
            uint32_t offset;
            uint32_t twiddle = 0U;

            for (offset = 0U; offset < half; ++offset)
            {
                uint32_t top = block + offset;
                uint32_t bottom = top + half;
                float weight_re = twiddle_re[twiddle];
                float weight_im = twiddle_im[twiddle];
                float product_re = (real[bottom] * weight_re) - (imag[bottom] * weight_im);
                float product_im = (real[bottom] * weight_im) + (imag[bottom] * weight_re);

                real[bottom] = real[top] - product_re;
                imag[bottom] = imag[top] - product_im;
                real[top] += product_re;
                imag[top] += product_im;
                twiddle += twiddle_step;
            }
        }
    }

    for (bin = 0U; bin < MEL_SPECTRUM_BINS; ++bin)
    {
        float magnitude = sqrtf((real[bin] * real[bin]) + (imag[bin] * imag[bin]));

        power[bin] = magnitude * magnitude;
    }
}
