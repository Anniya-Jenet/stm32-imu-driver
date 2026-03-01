/**
 * linker.ld — STM32F411xE linker script
 *
 * Flash : 512 KB @ 0x08000000
 * SRAM  : 128 KB @ 0x20000000
 */

ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
    /* ---- Vector table + code ---- */
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* ---- Initialised data (LMA in Flash, VMA in SRAM) ---- */
    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT > FLASH

    /* ---- Zero-initialised data ---- */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    /* ---- Stack (grows downward from top of SRAM) ---- */
    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        *(.stack)
    } > SRAM

    /* ---- Heap ---- */
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        *(.heap)
    } > SRAM

    /* Discard C++ exception tables if unused */
    /DISCARD/ :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }
}
