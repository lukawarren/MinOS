ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
	. = 1M;
	
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

	/* Make room for TSS stack (which grows downwards, of course) */
    . += 4k;
    __tss_stack = .;
    . += 16;

    /* Beginning of heap */
    . = ALIGN(4k);
    __kernel_end = .;
}
