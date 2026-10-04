/* SPDX-FileCopyrightText: 2024 Microsoft Corporation
 * SPDX-FileCopyrightText: 2026-present Eclipse ThreadX contributors
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: MIT */

/* STM32N6570-DK low-level ThreadX startup.  Adapted from the upstream
 * ThreadX/ports/cortex_m55/gnu/example_build/tx_initialize_low_level.S: this
 * project owns the vector symbol and linker layout. */

#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

.syntax unified
.arch armv8.1-m.main
.fpu fpv5-d16
.thumb

.equ SYSTEM_CLOCK, 600000000
.equ SYSTICK_CYCLES, ((SYSTEM_CLOCK / TX_TIMER_TICKS_PER_SECOND) - 1)

.section .text
.balign 4
.global _tx_initialize_low_level
.thumb_func
.type _tx_initialize_low_level, %function
_tx_initialize_low_level:
    cpsid i

    /* First free byte of the linker RAM region, past .bss and .data (unused by the app). */
    ldr r0, =_tx_initialize_unused_memory
    ldr r1, =_edata
    str r1, [r0]

    ldr r0, =0xE000ED08
    ldr r1, =g_pfnVectors
    str r1, [r0]

    ldr r0, =0xE0001000
    ldr r1, [r0]
    orr r1, r1, #1
    str r1, [r0]

    ldr r0, =_tx_thread_system_stack_ptr
    ldr r1, =g_pfnVectors
    ldr r1, [r1]
    str r1, [r0]

    ldr r0, =0xE000E000
    ldr r1, =SYSTICK_CYCLES
    str r1, [r0, #0x14]
    movs r1, #7
    str r1, [r0, #0x10]

    /* SVC and PendSV lowest; SysTick above PendSV. */
    ldr r1, =0xFF000000
    str r1, [r0, #0xD1C]
    ldr r1, =0x40FF0000
    str r1, [r0, #0xD20]

    /* From this point the SysTick handler may safely call ThreadX. */
    ldr r0, =g_threadx_kernel_started
    movs r1, #1
    str r1, [r0]
    bx lr
.size _tx_initialize_low_level, . - _tx_initialize_low_level

.section .text
.balign 4
.global SysTick_Handler
.thumb_func
.type SysTick_Handler, %function
SysTick_Handler:
    push {r0, lr}
    ldr r0, =g_threadx_kernel_started
    ldr r1, [r0]
    cbz r1, 1f
    bl _tx_timer_interrupt
1:
    bl HAL_IncTick
    pop {r0, lr}
    bx lr
.size SysTick_Handler, . - SysTick_Handler

.end
