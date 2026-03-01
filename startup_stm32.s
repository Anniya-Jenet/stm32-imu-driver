/**
 * startup_stm32f411.s
 * Minimal startup file for STM32F411xE (Cortex-M4)
 * Toolchain: arm-none-eabi-gcc / GNU as
 */

    .syntax unified
    .cpu cortex-m4
    .fpu softvfp
    .thumb

/* ---- stack and heap sizes ---- */
    .equ  Stack_Size, 0x400   /* 1 KB */
    .equ  Heap_Size,  0x200   /* 512 B */

/* ---- stack section ---- */
    .section .stack, "w", %nobits
    .align 3
Stack_Mem:
    .space Stack_Size
__StackTop:

/* ---- heap section ---- */
    .section .heap, "w", %nobits
    .align 3
Heap_Mem:
    .space Heap_Size
__HeapLimit:

/* ---- vector table ---- */
    .section .isr_vector, "a", %progbits
    .type  g_pfnVectors, %object

g_pfnVectors:
    .word  __StackTop               /* Stack pointer */
    .word  Reset_Handler            /* Reset          */
    .word  NMI_Handler
    .word  HardFault_Handler
    .word  MemManage_Handler
    .word  BusFault_Handler
    .word  UsageFault_Handler
    .word  0
    .word  0
    .word  0
    .word  0
    .word  SVC_Handler
    .word  DebugMon_Handler
    .word  0
    .word  PendSV_Handler
    .word  SysTick_Handler
    /* External interrupts (IRQ0..IRQ85 for STM32F411) */
    .rept  96
    .word  Default_Handler
    .endr

/* ---- Reset handler ---- */
    .section .text.Reset_Handler, "ax", %progbits
    .type Reset_Handler, %function
    .global Reset_Handler
Reset_Handler:
    /* Copy .data section from Flash to SRAM */
    ldr   r0, =_sdata
    ldr   r1, =_edata
    ldr   r2, =_sidata
    movs  r3, #0
    b     .Lloop_copy

.Lcopy_data:
    ldr   r4, [r2, r3]
    str   r4, [r0, r3]
    adds  r3, r3, #4

.Lloop_copy:
    adds  r4, r0, r3
    cmp   r4, r1
    bcc   .Lcopy_data

    /* Zero-fill .bss section */
    ldr   r2, =_sbss
    ldr   r4, =_ebss
    movs  r3, #0
    b     .Lloop_bss

.Lzero_bss:
    str   r3, [r2]
    adds  r2, r2, #4

.Lloop_bss:
    cmp   r2, r4
    bcc   .Lzero_bss

    /* Call main */
    bl    main
    bx    lr

/* ---- Default / weak handlers ---- */
    .section .text, "ax", %progbits

    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    b .

    .weak HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    b .

    .weak MemManage_Handler
    .type MemManage_Handler, %function
MemManage_Handler:
    b .

    .weak BusFault_Handler
    .type BusFault_Handler, %function
BusFault_Handler:
    b .

    .weak UsageFault_Handler
    .type UsageFault_Handler, %function
UsageFault_Handler:
    b .

    .weak SVC_Handler
    .type SVC_Handler, %function
SVC_Handler:
    b .

    .weak DebugMon_Handler
    .type DebugMon_Handler, %function
DebugMon_Handler:
    b .

    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    b .

    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    b .

    .type Default_Handler, %function
Default_Handler:
    b .

    .end
