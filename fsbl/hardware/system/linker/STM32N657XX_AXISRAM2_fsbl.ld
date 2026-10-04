/*
******************************************************************************
**
** @file        : STM32N657XX_AXISRAM2_fsbl.ld
**
** @author      : GPM Application Team
**
** @brief       : Linker script for STM32N657XX Device from STM32N6 series
**                (AXISRAM2, the FSBL image, 511 KB)
**
**                The bootROM loads this image into AXISRAM2 at 0x34180400.
**                Once the FSBL jumps to the application, this range is free:
**                it becomes part of the NPU activation pools (see the memory
**                map in src/hardware/mcu/system/linker/STM32N657XX_XIP.ld).
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
******************************************************************************
** @attention
**
** Copyright (c) 2023 STMicroelectronics.
** Modifications Copyright (c) 2026 Nikita Prismakov.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
******************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/*
 * Memory map: the whole FSBL lives in AXISRAM2, where the bootROM loads it
 * (code, constants and data alike: there is no separate load image).
 *
 *   region     start        end          size    what lives there
 *   ---------  -----------  -----------  ------  -----------------------------
 *   RAM        0x34180400   0x34200000   511 KB  code, constants, .data, .bss,
 *                                                the main stack (2 KB)
 *
 * There is no C library heap: the FSBL does not allocate.
 */
MEMORY
{
  RAM   (xrw) : ORIGIN = 0x34180400, LENGTH = 0x0007FC00
}

/* Size of the main stack. It follows .bss in RAM, see .stack. */
_stack_size = 0x800;

/* Sections */
SECTIONS
{
  /* ------------------------------ RAM ---------------------------------- */

  /* Vector table first: the bootROM starts the image through it. */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >RAM

  /* Program code; .RamFunc (__RAM_FUNC) needs no copy, everything is in RAM. */
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)
    *(.RamFunc)
    *(.RamFunc*)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;
  } >RAM

  /* Constant data. */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >RAM

  .ARM.extab : {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } >RAM

  .ARM : {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >RAM

  .preinit_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >RAM

  .init_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >RAM

  .fini_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >RAM

  /* Initialized data. The bootROM already loaded it in place, so the
     startup's _sidata -> _sdata copy moves it onto itself. */
  _sidata = LOADADDR(.data);
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM

  .gnu.sgstubs :
  {
    . = ALIGN(4);
    *(.gnu.sgstubs*)   /* Secure Gateway stubs */
    . = ALIGN(4);
  } >RAM

  /* Uninitialized data, zeroed by the startup (_sbss.._ebss). */
  . = ALIGN(4);
  .bss :
  {
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM

  /* ---------------------------- main stack ---------------------------- */

  /* Nothing is linked into this section: it only reserves the space behind
     .bss, so the link fails if RAM cannot hold it too. The startup loads
     MSPLIM from _sstack and SP from _estack. */
  .stack (NOLOAD) :
  {
    . = ALIGN(8);
    _sstack = .;
    . = . + _stack_size;
    _estack = .;
  } >RAM

  /* Drop what the C, math and gcc libraries bring that no rule above placed
     (their .ARM.attributes, debug information, ...). */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
