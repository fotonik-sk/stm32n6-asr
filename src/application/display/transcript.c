/**
 * @file     transcript.c
 * @brief    The recognized text as it appears on the LCD; implements transcript.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "transcript.h"

#include <stdio.h>
#include <string.h>

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/** @brief Font scale of the text. */
#define TRANSCRIPT_TEXT_SCALE      1U

/** @brief Blank border around the text, in pixels. */
#define TRANSCRIPT_MARGIN_PIXELS   16U

/** @brief Final characters kept; far more than the screen shows. */
#define TRANSCRIPT_CAPACITY        1024U

/** @brief How far past the minimum cut transcript_append() looks for a word boundary. */
#define TRANSCRIPT_WORD_SEARCH_CHARS 32U

/** @brief RGB565 grey of the tentative tail. */
#define TRANSCRIPT_COLOR_TENTATIVE 0x8410U

/*******************************************************************************/
/*                              PRIVATE VARIABLES                              */
/*******************************************************************************/

/** @brief Final text, NUL-terminated. */
static char transcript[TRANSCRIPT_CAPACITY];

/** @brief Characters in @c transcript. */
static uint32_t transcript_length;

/** @brief Tentative tail drawn after the final text, NUL-terminated. */
static char tentative[TRANSCRIPT_TENTATIVE_CAPACITY];

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/** @brief Character @p index of the final text followed by the tentative tail. */
static char text_at(uint32_t index)
{
  return (index < transcript_length) ? transcript[index]
                                     : tentative[index - transcript_length];
}

/** @brief Advance of one character at the text scale. */
static uint32_t char_width(char character)
{
  const char text[2] = { character, '\0' };

  return mw_lcd_measure_string_width(text, TRANSCRIPT_TEXT_SCALE);
}

/**
 * @brief Word-wrap the text into lines and optionally draw them.
 * @param[in] total Characters of final text plus tentative tail.
 * @param[in] skip_lines Lines above the screen; they are laid out, not drawn.
 * @param[in] draw Zero to only count lines.
 * @return Lines the text occupies.
 */
static uint32_t layout_text(uint32_t total, uint32_t skip_lines, int draw)
{
  const uint32_t width = (uint32_t)mw_lcd_get_width() - (2U * TRANSCRIPT_MARGIN_PIXELS);
  const uint32_t line_height = mw_lcd_get_line_height(TRANSCRIPT_TEXT_SCALE);
  uint32_t x = 0U;
  uint32_t line = 0U;
  uint32_t index = 0U;

  if (total == 0U)
  {
    return 0U;
  }
  while (index < total)
  {
    uint32_t word_end = index;
    uint32_t word_width = 0U;

    if (text_at(index) == ' ')
    {
      /* A space separates words but never starts a line. */
      if (x > 0U)
      {
        x += char_width(' ');
        if (x > width)
        {
          x = 0U;
          ++line;
        }
      }
      ++index;
      continue;
    }

    while ((word_end < total) && (text_at(word_end) != ' '))
    {
      word_width += char_width(text_at(word_end));
      ++word_end;
    }
    if ((x > 0U) && ((x + word_width) > width))
    {
      x = 0U;
      ++line;
    }
    for (; index < word_end; ++index)
    {
      const char character = text_at(index);
      const uint32_t advance = char_width(character);

      /* Only a word wider than the screen is broken inside. */
      if ((x > 0U) && ((x + advance) > width))
      {
        x = 0U;
        ++line;
      }
      if (draw && (line >= skip_lines))
      {
        /* Black for final text, grey for the tentative tail. */
        mw_lcd_draw_char((uint16_t)(TRANSCRIPT_MARGIN_PIXELS + x),
                         (uint16_t)(TRANSCRIPT_MARGIN_PIXELS + ((line - skip_lines) * line_height)),
                         character,
                         TRANSCRIPT_TEXT_SCALE,
                         (index < transcript_length) ? MW_LCD_COLOR_BLACK : TRANSCRIPT_COLOR_TENTATIVE,
                         true);
      }
      x += advance;
    }
  }
  return line + 1U;
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void transcript_append(const char *text, uint32_t length)
{
  const uint32_t room = TRANSCRIPT_CAPACITY - 1U;

  /* A text longer than the whole buffer: keep only its newest characters. */
  if (length > room)
  {
    text += length - room;
    length = room;
  }
  /* Not enough room left: drop the oldest characters first. */
  if ((transcript_length + length) > room)
  {
    const uint32_t min_drop = (transcript_length + length) - room;
    uint32_t drop = min_drop;

    /* Cut at a word boundary when one is near, so no half word remains. */
    while ((drop < transcript_length) && (transcript[drop] != ' ') &&
           (drop < (min_drop + TRANSCRIPT_WORD_SEARCH_CHARS)))
    {
      ++drop;
    }
    if (drop > transcript_length)
    {
      drop = transcript_length;
    }
    (void)memmove(transcript, &transcript[drop], transcript_length - drop);
    transcript_length -= drop;
  }
  (void)memcpy(&transcript[transcript_length], text, length);
  transcript_length += length;
  transcript[transcript_length] = '\0';
}

void transcript_set_tentative(const char *text)
{
  (void)snprintf(tentative, sizeof(tentative), "%s", text);
}

int transcript_has_text(void)
{
  const uint32_t total = transcript_length + (uint32_t)strlen(tentative);
  uint32_t index;

  for (index = 0U; index < total; ++index)
  {
    if (text_at(index) != ' ')
    {
      return 1;
    }
  }
  return 0;
}

mw_lcd_status_t transcript_draw(void)
{
  const uint32_t total = transcript_length + (uint32_t)strlen(tentative);
  const uint32_t visible = ((uint32_t)mw_lcd_get_height() - (2U * TRANSCRIPT_MARGIN_PIXELS)) /
                           mw_lcd_get_line_height(TRANSCRIPT_TEXT_SCALE);
  const uint32_t lines = layout_text(total, 0U, 0);
  const uint32_t skip = (lines > visible) ? (lines - visible) : 0U;

  mw_lcd_clear(MW_LCD_COLOR_WHITE);
  (void)layout_text(total, skip, 1);
  return mw_lcd_present();
}
