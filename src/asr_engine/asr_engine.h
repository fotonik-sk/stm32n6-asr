/**
 * @file     asr_engine.h
 * @brief    Public API of the asr_engine layer: two ports and three entry points.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ASR_ENGINE_ASR_ENGINE_H
#define ASR_ENGINE_ASR_ENGINE_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#include "asr_engine_conf.h"
#include "asr_model_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                                 PUBLIC TYPES                                */
/*******************************************************************************/

/** @brief Result of an audio-port operation. */
typedef enum
{
    ASR_AUDIO_OK = 0,   /**< The operation completed. */
    ASR_AUDIO_ERROR,    /**< Invalid argument, or the capture driver is not running. */
    ASR_AUDIO_UNDERRUN, /**< Fewer complete chunks are stored than were requested. */
    ASR_AUDIO_OVERRUN   /**< Capture reached the requested chunks while they were copied. */
} asr_audio_status_t;

/**
 * @brief Capture backend the layer reads, filled in by the host project.
 */
typedef struct
{
    /** @brief Opaque handle passed back to every function below. */
    void *context;

    /**
     * @brief Return the number of complete chunks waiting to be read.
     * @param[in,out] context @c context above.
     * @return Stored chunk count; 0 when the capture ring is empty.
     */
    uint32_t (*available_chunks)(void *context);

    /**
     * @brief Copy the oldest complete chunks out of the capture ring and release them.
     * @param[in,out] context @c context above.
     * @param[out] destination Receives the PCM16 samples.
     * @param[in] destination_samples Capacity of @p destination in samples.
     * @param[in] chunks Chunks to copy, 1 .. @c ASR_CONF_CAPTURE_CHUNK_COUNT - 1.
     * @retval ASR_AUDIO_OK Every chunk was copied and released.
               ASR_AUDIO_ERROR Invalid argument, or @p destination is too small.
               ASR_AUDIO_UNDERRUN Fewer chunks are stored; nothing was copied.
               ASR_AUDIO_OVERRUN Capture overwrote the chunks during the copy;
               the samples are discarded.
     * @note Called only from the mel thread.
     */
    asr_audio_status_t (*read)(void *context,
                               int16_t *destination,
                               uint32_t destination_samples,
                               uint32_t chunks);

    /**
     * @brief Read and clear the number of chunks lost since the previous call.
     * @param[in,out] context @c context above.
     * @return Chunks overwritten before they were read.
     */
    uint32_t (*take_dropped_chunks)(void *context);
} asr_audio_port_t;

/**
 * @brief Destination of the recognized text, filled in by the host project.
 * @note The text is upper-case letters, apostrophes and spaces.
 */
typedef struct
{
    /** @brief Opaque handle passed back to @c publish. */
    void *context;

    /**
     * @brief Deliver the text of one network window.
     * @param[in,out] context @c context above.
     * @param[in] committed Newly final text; append it to the transcript. May
     *            be empty or cut a word, since windows ignore word boundaries.
     * @param[in] tentative Current guess for the right context; replaces the
     *            previous one. May be empty.
     * @note Called from the network thread every @c ASR_MODEL_NET_STRIDE_FRAMES
     *       frames. Both strings are valid only during the call.
     */
    void (*publish)(void *context, const char *committed, const char *tentative);
} asr_text_port_t;

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Create the threads and RTOS objects of every stage, suspended.
 * @return TX_SUCCESS, or the ThreadX error of the failing step.
 * @pre Call from tx_application_define().
 */
uint32_t asr_engine_create(void);

/**
 * @brief Initialize every stage and resume its thread, the last stage first.
 * @param[in] audio Capture backend; not NULL, must outlive the layer.
 * @param[in] text Text destination, or NULL to only log it; must outlive the layer.
 * @return TX_SUCCESS, or non-zero on failure; the reason is logged.
 * @note The first text comes @c ASR_MODEL_NET_STRIDE_FRAMES frames plus one
 *       inference later: the window starts padded with silence.
 */
uint32_t asr_engine_start(const asr_audio_port_t *audio, const asr_text_port_t *text);

/**
 * @brief Notify the layer that one capture chunk is complete.
 * @note Interrupt-safe and cheap; harmless before asr_engine_start().
 */
void asr_engine_audio_chunk_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* ASR_ENGINE_ASR_ENGINE_H */
