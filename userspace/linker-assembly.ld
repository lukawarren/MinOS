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

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data) *(.data.*)
    }
}