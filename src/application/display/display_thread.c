/**
 * @file     display_thread.c
 * @brief    Display thread and the asr_engine text port that feeds it.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "display_thread.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "app_threads.h"
#include "bsp_lcd.h"
#include "drv_ltdc.h"
#include "logger.h"
#include "mw_lcd.h"
#include "startup_logo.h"
#include "transcript.h"
#include "tx_api.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief display_thread_start() status for a failed non-RTOS step; no TX_* code uses it. */
#define DISPLAY_START_ERROR 0xFFFFFFFFU

/** @brief Final characters that may queue up in the mailbox between two frames. */
#define DISPLAY_PENDING_CAPACITY 512U

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Control block of the display thread. */
static TX_THREAD display_thread;

/** @brief Private stack of the display thread. */
static ULONG display_thread_stack[APP_DISPLAY_THREAD_STACK_BYTES / sizeof(ULONG)];

/** @brief Guards the mailbox shared with the asr_engine network thread. */
static TX_MUTEX mailbox_mutex;

/** @brief Raised when the mailbox changed; holds at most one token. */
static TX_SEMAPHORE mailbox_semaphore;

/** @brief Mailbox: final text not drawn yet, NUL-terminated. */
static char mailbox_final[DISPLAY_PENDING_CAPACITY];

/** @brief Mailbox: characters in @c mailbox_final. */
static uint32_t mailbox_final_length;

/** @brief Mailbox: newest tentative tail, NUL-terminated. */
static char mailbox_tentative[TRANSCRIPT_TENTATIVE_CAPACITY];

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/** @brief Hand the finished frame to the LTDC; the mw_lcd present callback. */
static uint16_t *present_frame(void *context)
{
  (void)context;
  return drv_ltdc_present();
}

/** @brief Start the LCD renderer on the panel geometry and buffers of the LTDC. */
static mw_lcd_status_t lcd_renderer_init(void)
{
  mw_lcd_config_t config = {0};

  config.width = drv_ltdc_get_width();
  config.height = drv_ltdc_get_height();
  config.stride_pixels = drv_ltdc_get_width(); /* The LTDC rows are packed. */
  config.draw_buffer = drv_ltdc_get_draw_buffer();
  config.present_callback = present_frame;
  config.callback_context = NULL;
  return mw_lcd_init(&config);
}

/**
 * @brief Take the text of one window; runs on the asr_engine network thread.
 * @note Only copies into the mailbox and wakes the display thread.
 */
static void publish_text(void *context, const char *final_text, const char *tentative_text)
{
  uint32_t length = (uint32_t)strlen(final_text);
  uint32_t dropped = 0U;
  uint32_t room;

  (void)context;
  if (tx_mutex_get(&mailbox_mutex, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    return;
  }

  /* If the display fell that far behind, the excess final text is lost.
     One byte of the capacity is kept for the NUL. */
  room = DISPLAY_PENDING_CAPACITY - 1U - mailbox_final_length;
  if (length > room)
  {
    dropped = length - room;
    length = room;
  }
  (void)memcpy(&mailbox_final[mailbox_final_length], final_text, length);
  mailbox_final_length += length;
  mailbox_final[mailbox_final_length] = '\0';
  (void)snprintf(mailbox_tentative, sizeof(mailbox_tentative), "%s", tentative_text);

  (void)tx_mutex_put(&mailbox_mutex);
  (void)tx_semaphore_ceiling_put(&mailbox_semaphore, 1U);
  if (dropped > 0U)
  {
    LOG_WARNING("Display mailbox full: dropped %lu final characters",
                (unsigned long)dropped);
  }
}

/** @brief Move the mailbox contents into the transcript. */
static void take_mailbox(void)
{
  if (tx_mutex_get(&mailbox_mutex, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    return;
  }
  transcript_append(mailbox_final, mailbox_final_length);
  transcript_set_tentative(mailbox_tentative);
  mailbox_final_length = 0U;
  mailbox_final[0] = '\0';
  (void)tx_mutex_put(&mailbox_mutex);
}

/**
 * @brief Redraw the transcript whenever the recognized text changes.
 * @note Nothing is drawn until the first non-space character arrives, so the
 *       startup logo stays until then.
 */
static void display_thread_entry(ULONG argument)
{
  int text_started = 0;

  (void)argument;
  for (;;)
  {
    if (tx_semaphore_get(&mailbox_semaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
    {
      continue;
    }
    take_mailbox();
    if (!text_started)
    {
      if (!transcript_has_text())
      {
        continue;
      }
      text_started = 1;
    }
    if (transcript_draw() != MW_LCD_OK)
    {
      LOG_ERROR("LCD transcript presentation failed");
    }
  }
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

uint32_t display_thread_create(void)
{
  UINT status;

  status = tx_mutex_create(&mailbox_mutex, "display_mailbox", TX_INHERIT);
  if (status != TX_SUCCESS)
  {
    return (uint32_t)status;
  }
  status = tx_semaphore_create(&mailbox_semaphore, "display_update", 0U);
  if (status != TX_SUCCESS)
  {
    return (uint32_t)status;
  }
  return (uint32_t)tx_thread_create(&display_thread,
                                    "display_thread",
                                    display_thread_entry,
                                    0U,
                                    display_thread_stack,
                                    sizeof(display_thread_stack),
                                    APP_DISPLAY_THREAD_PRIORITY,
                                    APP_DISPLAY_THREAD_PRIORITY,
                                    TX_NO_TIME_SLICE,
                                    TX_DONT_START);
}

uint32_t display_thread_start(void)
{
  /* PSRAM, which holds the framebuffers, is already mapped by bsp_init(). */
  if (bsp_lcd_init() != HAL_OK)
  {
    LOG_ERROR("LCD initialization failed");
    return DISPLAY_START_ERROR;
  }
  if (lcd_renderer_init() != MW_LCD_OK)
  {
    LOG_ERROR("LCD renderer initialization failed");
    return DISPLAY_START_ERROR;
  }
  if (startup_logo_show() != 0U)
  {
    return DISPLAY_START_ERROR;
  }

  /* From here on only the display thread draws. */
  return (uint32_t)tx_thread_resume(&display_thread);
}

const asr_text_port_t *display_text_port_get(void)
{
  /** @brief The display's plug into the model layer. */
  static const asr_text_port_t text_port = {
    .context = NULL,
    .publish = publish_text,
  };

  return &text_port;
}
