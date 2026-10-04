/**
 * @file     network_shape.h
 * @brief    Interface of the compiled CynipsNet network.
 * @details  asr_model_params.h takes the model geometry from this interface.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_NETWORK_SHAPE_H
#define ASR_ENGINE_NETWORK_SHAPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/** @brief Public name of the compiled network. */
#define ASR_NETWORK_NAME "cynipsnet"

/** @brief Input frames: the tensor is int8 [frames][bands]. */
#define ASR_NETWORK_INPUT_FRAMES 600U

/** @brief Bands per input frame. */
#define ASR_NETWORK_INPUT_BANDS 64U

/** @brief Quantization scale of the input. */
#define ASR_NETWORK_INPUT_SCALE 0.0628561676f

/** @brief Quantization zero point of the input. */
#define ASR_NETWORK_INPUT_ZERO_POINT 0

/** @brief Output time steps: the tensor is int8 [steps][classes]. */
#define ASR_NETWORK_OUTPUT_STEPS 300U

/** @brief Output classes per step. */
#define ASR_NETWORK_OUTPUT_CLASSES 29U

/** @brief Quantization scale of the output; the zero point is 0. */
#define ASR_NETWORK_OUTPUT_SCALE 0.267144024f

/**
 * @brief Bytes from the input tensor to the end of its memory pool (npuRAM5).
 * @note The input tensor starts at 0x342E0000.
 */
#define ASR_NETWORK_INPUT_POOL_BYTES 458752U

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_NETWORK_SHAPE_H */
