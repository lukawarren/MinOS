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

    /* remove unnecessary bits */
    /DISCARD/ :
	{
		*(.comment)
		*(.note.gnu.build-id)
		*(.gnu.build.attributes)
	}
}
