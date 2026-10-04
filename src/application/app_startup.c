/**
 * @file     app_startup.c
 * @brief    Start-up sequence of the firmware: which module starts when.
 * @details  Both phases walk the table @c modules: @c create before the
 *           scheduler (RTOS objects only, no hardware), then @c start in the
 *           startup thread, in table order. The startup thread then exits.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "app_startup.h"

#include <stddef.h>
#include <stdint.h>

#include "app_threads.h"
#include "asr_engine.h"
#include "audio_port.h"
#include "boards.h"
#include "display_thread.h"
#include "drv_npu.h"
#include "drv_uart.h"
#include "led_thread.h"
#include "logger.h"
#include "main.h"
#include "tx_api.h"
#include "version.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief What every create/start returns on success (TX_SUCCESS, DRV_PDM_OK, ...). */
#define APP_MODULE_OK 0U

/** @brief Number of entries in the start-up table. */
#define APP_MODULE_COUNT (sizeof(modules) / sizeof(modules[0]))

/*******************************************************************************/
/*                                PRIVATE TYPES                                */
/*******************************************************************************/

/** @brief One line of the start-up table. */
typedef struct
{
  /** @brief Module name, shown when it fails to start. */
  const char *name;

  /** @brief Phase 1, before the scheduler: create RTOS objects; NULL if none. */
  uint32_t (*create)(void);

  /** @brief Phase 2, in the startup thread: bring the module up; NULL if nothing to do. */
  uint32_t (*start)(void);
} app_module_t;

/*******************************************************************************/
/*                         PRIVATE FUNCTION PROTOTYPES                         */
/*******************************************************************************/

/* Needed because the table below refers to them. */
static uint32_t logger_start(void);
static uint32_t npu_start(void);
static uint32_t recognition_start(void);

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/**
 * @brief The start-up table: every module of the firmware, in boot order.
 * @note Dependency order: each module may rely on those above it.
 */
static const app_module_t modules[] = {
  /* name         phase 1: create         phase 2: start        */
  { "logger",     NULL,                   logger_start          },
  { "display",    display_thread_create,  display_thread_start  },
  { "led",        led_thread_create,      led_thread_start      },
  { "npu",        NULL,                   npu_start             },
  { "microphone", NULL,                   audio_port_start      },
  { "asr_engine",  asr_engine_create,       recognition_start     },
};

/** @brief Control block of the startup thread. */
static TX_THREAD startup_thread;

/** @brief Private stack of the startup thread. */
static ULONG startup_thread_stack[APP_STARTUP_THREAD_STACK_BYTES / sizeof(ULONG)];

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/** @brief Attach the logger to the console UART that bsp_init() opened. */
static uint32_t logger_start(void)
{
  if (logger_init(drv_uart_write, drv_uart_tx_ready) != LOGGER_OK)
  {
    return 1U;
  }
  LOG_INFO("%s, firmware v%s, model %s v%s", BOARD_NAME, VERSION_STRING, ASR_MODEL_NAME,
           ASR_MODEL_VERSION);
  return APP_MODULE_OK;
}

/** @brief Power and clock the NPU; asr_engine only runs the network on it. */
static uint32_t npu_start(void)
{
  drv_npu_init();
  return APP_MODULE_OK;
}

/** @brief Start recognition: microphone audio in, text out to the display. */
static uint32_t recognition_start(void)
{
  return asr_engine_start(audio_port_get(), display_text_port_get());
}

/** @brief Phase 2: start every module in table order, then exit. */
static void startup_thread_entry(ULONG argument)
{
  uint32_t index;

  (void)argument;
  for (index = 0U; index < APP_MODULE_COUNT; ++index)
  {
    const app_module_t *module = &modules[index];

    if (module->start == NULL)
    {
      continue;
    }
    const uint32_t status = module->start();
    if (status != APP_MODULE_OK)
    {
      (void)LOG_CRITICAL("Start-up failed: %s (status %lu)", module->name, (unsigned long)status);
      error_handler();
    }
  }

  /* Returning from a ThreadX entry function terminates this thread. */
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void tx_application_define(void *first_unused_memory)
{
  uint32_t index;

  (void)first_unused_memory;

  /* Phase 1: create every module's RTOS objects, suspended. */
  for (index = 0U; index < APP_MODULE_COUNT; ++index)
  {
    if ((modules[index].create != NULL) && (modules[index].create() != APP_MODULE_OK))
    {
      error_handler();
    }
  }

  /* The one thread that starts running: it performs phase 2. */
  if (tx_thread_create(&startup_thread,
                       "startup_thread",
                       startup_thread_entry,
                       0U,
                       startup_thread_stack,
                       sizeof(startup_thread_stack),
                       APP_STARTUP_THREAD_PRIORITY,
                       APP_STARTUP_THREAD_PRIORITY,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    error_handler();
  }
}
