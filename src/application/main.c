/**
 * @file     main.c
 * @brief    Application entry point: platform bring-up, then the ThreadX kernel.
 * @details  Runs in place from external flash. bsp_init() sets up the platform;
 *           the modules are started from app_startup.c.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "main.h"

#include "bsp.h"
#include "drv_uart.h"
#include "logger.h"
#include "tx_api.h"

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Application entry point.
 */
int main(void)
{
  bsp_init();

  /* Never returns: the scheduler runs the threads from here on. */
  tx_kernel_enter();
  for (;;) { }
}

/**
 * @brief Executed on unrecoverable error.
 */
void error_handler(void)
{
  __disable_irq();
  (void)drv_uart_write_direct("\r\n[CRIT ] error_handler reached, halting\r\n");
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)LOG_ERROR("assert_failed %s:%lu", (const char *)file, (unsigned long)line);
}
#endif /* USE_FULL_ASSERT */
