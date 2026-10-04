/*
******************************************************************************
**
** @file        : STM32N657XX_XIP.ld
**
** @brief       : Application linker script for the STM32N657X0H3QU on the
**                STM32N6570-DK: execute in place from the external NOR,
**                data in AXISRAM1 and external PSRAM.
**
**                The MEMORY table below lists the four regions the
**                application links into. The code reads addresses only
**                through the symbols right after the table.
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
 * Memory map.
 *
 *   region         start        end          size    what lives there
 *   -------------  -----------  -----------  ------  ------------------------
 *   NOR_APP        0x70100400   0x71500000   ~20 MB  code, constants, .data
 *                                                    load image (the FSBL
 *                                                    jumps to 0x70100400)
 *   NONCACHEABLE   0x34000000   0x34002000     8 KB  DMA buffers (MPU region 0)
 *   RAM            0x34002000   0x34064000   392 KB  .bss, .data, the C
 *                                                    library heap (2 KB), the
 *                                                    main stack (2 KB)
 *   PSRAM          0x90000000   0x90500000     5 MB  LCD framebuffers,
 *                                                    .psram_bss
 *
 * The rest of the memory belongs to the NPU. The application links nothing
 * there, so it is not in the MEMORY table; the regions above end where it
 * begins:
 *
 *   0x71500000   0x78000000   107 MB  network weights, in the NOR
 *   0x34064000   0x343C0000  3440 KB  NPU activations
 *   0x343C0000   0x34400000   256 KB  the NPU cache itself: never data
 *   0x90500000   0x92000000    27 MB  NPU activations, in the PSRAM
 *
 * These are the memory pools the network was compiled with (the "offset="
 * comments at the top of src/asr_engine/models/<network>/network.c).
 */
MEMORY
{
  NOR_APP      (rx)  : ORIGIN = 0x70100400, LENGTH = 0x013FFC00
  NONCACHEABLE (rw)  : ORIGIN = 0x34000000, LENGTH = 0x00002000
  RAM          (rwx) : ORIGIN = 0x34002000, LENGTH = 0x00062000
  PSRAM        (rw)  : ORIGIN = 0x90000000, LENGTH = 0x00500000
}

/* Sizes of the C library heap (newlib-nano stdio buffers) and of the main
   stack (reset, ISRs). Both follow .data in RAM, see .heap and .msp_stack. */
_heap_size      = 0x800;
_msp_stack_size = 0x800;

/* Addresses handed to the code: start = first byte, end = first byte past
   the range. */

/* Non-cacheable DMA memory: mcu_core.c makes it MPU region 0. */
__noncacheable_start = ORIGIN(NONCACHEABLE);
__noncacheable_end   = ORIGIN(NONCACHEABLE) + LENGTH(NONCACHEABLE);

/* ES0620 2.2.18 NRST workaround (fsbl/application/main.c): the FSBL leaves a
   magic word here; the startup must overwrite it on every boot. It does,
   because the word lies in NONCACHEABLE, which the startup zeroes. */
__fsbl_nrst_marker = 0x34001004;

/* Sections */
SECTIONS
{
  /* ---------------------------- NOR_APP -------------------------------- */

  /* Vector table first: the FSBL jumps to ORIGIN(NOR_APP). */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >NOR_APP

  /* Program code, executed in place. */
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;
  } >NOR_APP

  /* Constant data, read in place. */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >NOR_APP

  .ARM.extab : {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } >NOR_APP

  .ARM : {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >NOR_APP

  .preinit_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >NOR_APP

  .init_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >NOR_APP

  .fini_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >NOR_APP

  .gnu.sgstubs :
  {
    . = ALIGN(4);
    *(.gnu.sgstubs*)   /* Secure Gateway stubs */
    . = ALIGN(4);
  } >NOR_APP

  /* ---------------------- NONCACHEABLE + RAM -------------------------- */

  /* The startup zeroes one range, _sbss.._ebss. It starts at the beginning
     of NONCACHEABLE and ends after .bss, which comes first in RAM right
     behind it, so the one loop zeroes both the DMA buffers and .bss. */

  /* Buffers marked __NON_CACHEABLE (DMA nodes, PDM capture). */
  .noncacheable (NOLOAD) :
  {
    _sbss = .;
    __bss_start__ = _sbss;
    KEEP(*(.noncacheable))
    KEEP(*(noncacheable_buffer))
  } >NONCACHEABLE

  /* Uninitialized data, zeroed by the startup. */
  .bss (NOLOAD) :
  {
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM

  /* Initialized data: stored in NOR, copied to RAM by the startup
     (_sidata -> _sdata.._edata). .RamFunc code (__RAM_FUNC) must run from
     SRAM, so it is copied along with the data. */
  _sidata = LOADADDR(.data);
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    *(.RamFunc)
    *(.RamFunc*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> NOR_APP

  /* ----------------------- heap, main stack ---------------------------- */

  /* Nothing is linked into these two sections: they only reserve the space
     behind .data, so the link fails if RAM cannot hold them too. */

  /* C library heap: sysmem.c hands out [__heap_start, __heap_end). end/_end
     are the names newlib and ThreadX samples expect for the heap start. */
  .heap (NOLOAD) :
  {
    . = ALIGN(8);
    __heap_start = .;
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _heap_size;
    __heap_end = .;
  } >RAM

  /* Main stack: the startup loads MSPLIM from _sstack and SP from _estack. */
  .msp_stack (NOLOAD) :
  {
    . = ALIGN(8);
    _sstack = .;
    . = . + _msp_stack_size;
    _estack = .;
  } >RAM

  /* ----------------------------- PSRAM --------------------------------- */

  /* PSRAM is mapped by the application in bsp_init(). The startup neither
     loads nor zeroes it: whatever lands here must write before it reads. */

  /* The two LCD framebuffers (bsp_lcd.c), cleared by the LTDC driver. */
  .lcd_framebuffer (NOLOAD) :
  {
    . = ALIGN(32);
    KEEP(*(.lcd_framebuffer))
    . = ALIGN(32);
  } >PSRAM

  /* CPU-only buffers too large for AXISRAM1
     (ASR_CONF_NORM_MEMORY_ATTRIBUTES). */
  .psram_bss (NOLOAD) :
  {
    . = ALIGN(32);
    *(.psram_bss)
    *(.psram_bss*)
    . = ALIGN(32);
  } >PSRAM

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

/* The single zeroing loop of the startup relies on this layout. */
ASSERT(ORIGIN(NONCACHEABLE) + LENGTH(NONCACHEABLE) == ORIGIN(RAM),
       "NONCACHEABLE must end where RAM begins")
ASSERT(ADDR(.bss) == ORIGIN(RAM), ".bss must be first in RAM")
ASSERT(__fsbl_nrst_marker >= _sbss && __fsbl_nrst_marker + 4 <= _ebss,
       "FSBL NRST marker is not zeroed by the startup")
