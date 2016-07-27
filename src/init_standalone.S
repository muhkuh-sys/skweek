	.section .init_code, "ax"
	.arm

	.extern skweek_standalone
	.extern stack_top

	.global start


@--------------------------------------

  .func start

start:
	@ Clear the .bss section (zero init).
	mov     r1, #0
	ldr     r2, =__bss_start__
	ldr     r3, =__bss_end__
clear_bss:
	cmp     r2, r3
	strlo   r1, [r2], #4
	blo     clear_bss

	bl      skweek_standalone
  .endfunc

@--------------------------------------

  .end

