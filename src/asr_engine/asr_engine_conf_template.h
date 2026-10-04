/**
 * @file     asr_engine_conf_template.h
 * @brief    Template of the host-owned asr_engine_conf.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_CONF_H
#define ASR_ENGINE_CONF_H

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/**
 * @brief Chunks the host capture ring holds, 10 ms each; at least 2.
 * @note The mel thread may be late by @c (count - 1) * 10 ms without losing audio.
 */
#define ASR_CONF_CAPTURE_CHUNK_COUNT 10U

/**
 * @brief Feature vectors the mel ring holds: 128 = 1.28 s, 32 KiB.
 * @note This is how late the normalization thread may run before frames are lost.
 */
#define ASR_CONF_FEATURE_RING_FRAMES 128U

/** @brief Stack of the mel stage thread in bytes. */
#define ASR_CONF_MEL_THREAD_STACK_BYTES 4096U

/** @brief ThreadX priority of the mel stage thread: below start-up, above idle work. */
#define ASR_CONF_MEL_THREAD_PRIORITY 8U

/** @brief Stack of the normalization stage thread in bytes. */
#define ASR_CONF_NORM_THREAD_STACK_BYTES 4096U

/** @brief ThreadX priority of the normalization thread: below mel, it has more slack. */
#define ASR_CONF_NORM_THREAD_PRIORITY 9U

/** @brief Stack of the network stage thread in bytes. */
#define ASR_CONF_NET_THREAD_STACK_BYTES 4096U

/** @brief ThreadX priority of the network thread: lowest, it mostly waits for the NPU. */
#define ASR_CONF_NET_THREAD_PRIORITY 11U

/**
 * @brief Section attribute of the two normalization buffers (about 210 KiB).
 * @note Only the CPU uses them, at frame rate, so slow external RAM is fine.
 *       Empty puts them in @c .bss.
 */
#define ASR_CONF_NORM_MEMORY_ATTRIBUTES

#endif /* ASR_ENGINE_CONF_H */
