/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * Modifications Copyright (c) 2026 Nikita Prismakov.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*
 * FSBL entry point. The bootROM loads this image into AXISRAM2 and runs it:
 * it sets up clocks and caches, maps the external NOR over XSPI2 and jumps to
 * the application in place (execute-in-place, see stm32_extmem_conf.h).
 */

/*******************************************************************************/
/*                                  INCLUDES                                   */
/*******************************************************************************/

#include "main.h"

#include "mcu_clock.h"
#include "stm32_extmem_conf.h"

/*******************************************************************************/
/*                               PRIVATE DEFINES                               */
/*******************************************************************************/

/**
 * @brief Word the NRST workaround marks; the application's startup zeroes it.
 * @note Also in STM32N657XX_XIP.ld (__fsbl_nrst_marker): change both together.
 */
#define NRST_WORKAROUND_MAGIC_ADDRESS 0x34001004UL

/** @brief Value that marks the corrective reset as done. */
#define NRST_WORKAROUND_MAGIC_VALUE   0x11223344UL

/** @brief Minimum NRST pulse duration, in RCC_RDCR_MRD units. */
#define NRST_MINIMUM_DURATION         0x05UL

/*******************************************************************************/
/*                                  VARIABLES                                  */
/*******************************************************************************/

/** @brief HAL handle of XSPI2, which drives the NOR; the ExtMem manager uses it. */
XSPI_HandleTypeDef nor_xspi;

/** @brief Milliseconds counted by HAL_GetTick(). */
static volatile uint32_t tick_ms;

/** @brief SysTick value at the previous HAL_GetTick() call. */
static uint32_t tick_last;

/** @brief CPU cycles counted but not yet a whole millisecond. */
static uint32_t tick_cycles;

/*******************************************************************************/
/*                              STATIC FUNCTIONS                               */
/*******************************************************************************/

/**
 * @brief Apply the STM32N6 NRST workaround from ES0620 section 2.2.18.
 * @details AXISRAM1 is cleared by a cold power-on reset but retained by the
 *          software reset below. The application later overwrites the marker,
 *          so every new external boot gets exactly one corrective power-on-type
 *          software reset.
 */
static void apply_nrst_workaround(void)
{
  volatile uint32_t *reset_marker = (volatile uint32_t *)NRST_WORKAROUND_MAGIC_ADDRESS;

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  HAL_SYSCFG_EnablePowerOnReset();
  MODIFY_REG(RCC->RDCR, RCC_RDCR_MRD, NRST_MINIMUM_DURATION << RCC_RDCR_MRD_Pos);

  if (*reset_marker != NRST_WORKAROUND_MAGIC_VALUE)
  {
    *reset_marker = NRST_WORKAROUND_MAGIC_VALUE;
    __DSB();
    __ISB();
    NVIC_SystemReset();
  }
}

/**
 * @brief Start SysTick as a free-running 24-bit down-counter, without interrupt,
 *        for the polled HAL_GetTick() below.
 */
static void tick_init(void)
{
  SysTick->LOAD = 0x00FFFFFFU;
  SysTick->VAL = 0U;
  /* Processor clock, counter enabled, interrupt disabled. */
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

  tick_last = SysTick->VAL;
  tick_cycles = 0U;
  tick_ms = 0U;
}

/** @brief Configure the red LED (PG10, active low) as an output, switched off. */
static void led_init(void)
{
  GPIO_InitTypeDef gpio = {0};

  __HAL_RCC_GPIOG_CLK_ENABLE();

  gpio.Pin = GPIO_PIN_10;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &gpio);

  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
}

/**
 * @brief Initialize XSPI2 for the MX66UW1G45G NOR on XSPIM port 2.
 * @note The clock and pins are set up by HAL_XSPI_MspInit().
 */
static void nor_xspi_init(void)
{
  XSPIM_CfgTypeDef manager_config = {0};

  nor_xspi.Instance = XSPI2;
  nor_xspi.Init.FifoThresholdByte = 4;
  nor_xspi.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
  nor_xspi.Init.MemoryType = HAL_XSPI_MEMTYPE_MACRONIX;
  nor_xspi.Init.MemorySize = HAL_XSPI_SIZE_1GB;
  nor_xspi.Init.ChipSelectHighTimeCycle = 2;
  nor_xspi.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
  nor_xspi.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
  nor_xspi.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
  nor_xspi.Init.ClockPrescaler = 0;
  nor_xspi.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
  nor_xspi.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_ENABLE;
  nor_xspi.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
  nor_xspi.Init.MaxTran = 0;
  nor_xspi.Init.Refresh = 0;
  nor_xspi.Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;
  if (HAL_XSPI_Init(&nor_xspi) != HAL_OK)
  {
    error_handler();
  }

  manager_config.nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
  manager_config.IOPort = HAL_XSPIM_IOPORT_2;
  manager_config.Req2AckTime = 1;
  if (HAL_XSPIM_Config(&nor_xspi, &manager_config, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    error_handler();
  }
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief FSBL entry point: bring up the clocks and the NOR, then boot the application.
 */
int main(void)
{
  apply_nrst_workaround();

  SCB_EnableICache();
  SCB_EnableDCache();

  /* HAL timeouts run on a polled SysTick counter (see HAL_GetTick()) that
     advances without any interrupt, so every HAL timeout loop, e.g. the XSPI
     BUSY wait in HAL_XSPI_Init(), ends. */
  tick_init();
  HAL_Init();

  led_init();
  /* Red on = FSBL alive; the LED is active low. */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);

  /* CPU 600 MHz from PLL1; see mcu_clock.c. */
  if (!mcu_clock_init_system_pll())
  {
    error_handler();
  }
  /* HAL_RCC_ClockConfig() leaves SystemCoreClock at the 64 MHz HSI value.
     Refresh it, so HAL_GetTick() counts milliseconds at the real CPU clock
     before any HAL timeout is used. */
  SystemCoreClockUpdate();

  /* Map the NOR and jump to the application in place; BOOT_Application()
     returns only on failure. */
  nor_xspi_init();
  (void)EXTMEM_Init(EXTMEMORY_1, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2));
  if (BOOT_Application() != BOOT_OK)
  {
    error_handler();
  }
  for (;;) { }
}

/**
 * @brief Keep the HAL off SysTick: tick_init() already runs it free, without interrupt.
 * @param[in] TickPriority Unused; there is no tick interrupt.
 * @return HAL_OK.
 * @note HAL_Init() and HAL_RCC_ClockConfig() call this. The weak HAL version
 *       would reload SysTick with a 1 ms period, and HAL_GetTick() below, which
 *       expects the 24-bit wrap, would then run many times too fast.
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  (void)TickPriority;
  return HAL_OK;
}

/**
 * @brief HAL time base: milliseconds from the free-running SysTick counter.
 * @return Milliseconds since tick_init().
 * @note Independent of any interrupt, so HAL timeout loops always progress.
 *       It is called far more often than the ~28 ms SysTick wrap at 600 MHz,
 *       so no wrap is missed.
 */
uint32_t HAL_GetTick(void)
{
  uint32_t now = SysTick->VAL;
  uint32_t cycles_per_ms = SystemCoreClock / 1000U;

  /* SysTick counts down; cycles since the last call, modulo the 24-bit wrap. */
  tick_cycles += (tick_last - now) & 0x00FFFFFFU;
  tick_last = now;

  if (cycles_per_ms == 0U)
  {
    cycles_per_ms = 1U;
  }
  while (tick_cycles >= cycles_per_ms)
  {
    tick_cycles -= cycles_per_ms;
    tick_ms++;
  }
  return tick_ms;
}

void error_handler(void)
{
  /* Keep the red LED (PG10, active low) on to signal a boot failure, then stop. */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief HAL parameter check failed: stop here.
 * @param[in] file Source file of the failed check.
 * @param[in] line Line of the failed check.
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */
