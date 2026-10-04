/**
 * @file     asr_engine_conf.h
 * @brief    This board's configuration of the asr_engine layer.
 * @author   Nikita Prismakov
 *
 * Created from src/asr_engine/asr_engine_conf_template.h.
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_CONF_H
#define ASR_ENGINE_CONF_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

/* Stacks and priorities of the stage threads, kept with every other thread. */
#include "app_threads.h"

/*******************************************************************************/
/*                                PUBLIC DEFINES                               */
/*******************************************************************************/

/**
 * @brief Chunks the capture ring holds, 10 ms each; at least 2.
 * @note chunks * 160 samples must fit DRV_PDM_DMA_CAPTURE_SAMPLES (1600).
 */
#define ASR_CONF_CAPTURE_CHUNK_COUNT 10U

/** @brief Feature vectors the mel ring holds: 128 = 1.28 s, 32 KiB. */
#define ASR_CONF_FEATURE_RING_FRAMES 128U

/**
 * @brief Puts the two normalization buffers (about 210 KiB) in external PSRAM.
 * @note They do not fit AXISRAM1. Only the CPU uses them, so they need no cache
 *       maintenance. @c .psram_bss is not zeroed at start-up; normalization
 *       writes the buffers before it reads them.
 */
#define ASR_CONF_NORM_MEMORY_ATTRIBUTES __attribute__((section(".psram_bss")))

#endif /* ASR_ENGINE_CONF_H */
