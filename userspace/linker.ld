ENTRY(_start)
STARTUP(../minlibc/build/i686-minos/lib/crt0.o)
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

    .tbss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss) *(.bss.*)
    }

    .init_array BLOCK(4K) : ALIGN(4K)
	{
		__init_array_start = .;
		KEEP (*(.ctor*))
		__init_array_end = .;
	}

	.fini_array BLOCK(4K) : ALIGN(4K)
	{
		__fini_array_start = .;
		KEEP (*(.dtor*))
		__fini_array_end = .;
	}
}