ENTRY(_start)

SECTIONS
{
    . = 0x40000000;

    .text : ALIGN(4K)
    {
        *(.multiboot)
        *(.text) *(.text.*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata) *(.rodata.*)
    }

    .data : ALIGN(4K)
    {
        *(.data) *(.data.*)
    }

    .data.rel.ro : ALIGN(4K)
    {
         *(.data.rel.ro.local*) *(.data.rel.ro .data.rel.ro.*)
     }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss) *(.bss.*)
    }

    .tbss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss) *(.bss.*)
    }

    .init_array : ALIGN(4K)
    {
        __init_array_start = .;
        KEEP (*(.ctor*))
        __init_array_end = .;
    }

    .fini_array : ALIGN(4K)
    {
        __fini_array_start = .;
        KEEP (*(.dtor*))
        __fini_array_end = .;
    }
}