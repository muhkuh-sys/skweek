	.section .init_code, "ax"
	.arm

	.extern skweek_main
	.extern stack_top

	.global start


   @ optional can be used by python script to define individual entry points for cortex CA9 core0/1
#if ASIC_TYP==4000
   @ define entry point for CA9_core0
	.global start_core0
   @ define entry point for CA9_core1
	.global start_core1
#endif

@--------------------------------------

  .func start

   @ optional can be used by python script to define individual entry points for cortex CA9 core0/1
#if ASIC_TYP==4000
   @ define entry point for CA9_core0
start_core0:
   @ define entry point for CA9_core1
start_core1:
#endif

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

	bl      skweek_main

	ldmfd   sp!, {r0, r1, r2, r3, lr}
	bx      lr


  .endfunc

@--------------------------------------

  .end

