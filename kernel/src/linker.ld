ENTRY(_start)
 
SECTIONS
{
	. = 1M;
	
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
	
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
	
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)

		/* Make room for TSS stack (which grows downwards, of course) */
		. += 4k;
        __tss_stack = .;
        . += 16;

		/* Beginning of heap */
		. = ALIGN(4k);
		__kernel_end = .;
	}
}