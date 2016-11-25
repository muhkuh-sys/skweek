	.section .init_code, "ax"

	.extern skweek_main
	.extern stack_top

	.global start

#include "asic_types.h"

@--------------------------------------

@ The Cortex-M4 in the netX90 is a THUMB-only CPU.
@ All others start in 32 bit mode.
#if ASIC_TYP==ASIC_TYP_NETX90_MPW
	.syntax unified
	.thumb
#else
	.arm
#endif

    .func start
    .type start, STT_FUNC
start:
	push    {r0, r1, r2, r3, lr}

	@ Clear the .bss section (zero init).
	mov     r1, #0
	ldr     r2, =__bss_start__
	ldr     r3, =__bss_end__
clear_bss:
	str     r1, [r2], #4
	cmp     r2, r3
	bmi     clear_bss

	bl      skweek_main

	pop     {r0, r1, r2, r3, lr}
	bx      lr
  .endfunc

@--------------------------------------

  .end

