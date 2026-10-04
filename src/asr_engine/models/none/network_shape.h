/**
 * @file     network_shape.h
 * @brief    Window geometry of a build without a network (ASR_MODEL_NETWORK=none).
 * @note     Every models/<name>/ directory provides this header; for a real
 *           network it is generated together with the compiled network.
 *           Here no network exists, so the values are chosen by hand: the
 *           front-end and the sink run with the 600-frame window of
 *           CynipsNet.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_NETWORK_SHAPE_H
#define ASR_ENGINE_NETWORK_SHAPE_H

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

#define ASR_NETWORK_NAME "none"
#define ASR_NETWORK_INPUT_FRAMES 600U
#define ASR_NETWORK_INPUT_BANDS 64U
#define ASR_NETWORK_INPUT_SCALE 0.0626919493f
#define ASR_NETWORK_INPUT_ZERO_POINT 0
#define ASR_NETWORK_OUTPUT_STEPS 300U
#define ASR_NETWORK_OUTPUT_CLASSES 29U
#define ASR_NETWORK_OUTPUT_SCALE 0.259670109f
#define ASR_NETWORK_INPUT_POOL_BYTES 458752U

#endif /* ASR_ENGINE_NETWORK_SHAPE_H */
