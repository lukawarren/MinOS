ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 0x40000000;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text) *(.text.*)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata) *(.rodata.*)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data) *(.data.*)
    }

    .data.rel.ro : ALIGN(4K)
    {
         *(.data.rel.ro.local*) *(.data.rel.ro .data.rel.ro.*)
     }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss) *(.bss.*)
    }
}