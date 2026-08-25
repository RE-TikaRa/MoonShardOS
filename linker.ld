ENTRY(kmain)

SECTIONS
{
    . = 0xffffffff80000000;

    .text : ALIGN(4K)
    {
        *(.text*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata*)
    }

    .data : ALIGN(4K)
    {
        *(.data*)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss*)
    }

    .requests : ALIGN(8)
    {
        __requests_start = .;
        KEEP(*(.requests))
        __requests_end = .;
    }
}