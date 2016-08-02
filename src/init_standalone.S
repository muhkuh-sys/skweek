	.section .init_code, "ax"
	.arm

	.extern skweek_standalone
	.extern stack_top

	.global start

@--------------------------------------

    .func start
    .type start, STT_FUNC
start:
	stmfd   sp!, {r0, r1, r2, r3, lr}

	@ Clear the .bss section (zero init).
	mov     r1, #0
	ldr     r2, =__bss_start__
	ldr     r3, =__bss_end__
clear_bss:
	cmp     r2, r3
	strlo   r1, [r2], #4
	blo     clear_bss

	bl      skweek_standalone

	ldmfd   sp!, {r0, r1, r2, r3, lr}
	bx      lr
  .endfunc

@--------------------------------------

  .end

