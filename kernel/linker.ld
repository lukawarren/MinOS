ENTRY(_start)

SECTIONS
{
	/* Begin at 1MiB */
	. = 1M;
 
	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
 
	/* Read-only data. */
	.rodata : ALIGN(4K)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		. = ALIGN(16);

        /* TSS stack (growing downwards, of course)
		. += 4k;
		_tss_stack = .;

        /* End of kernel; start of heap */
		. = ALIGN(4k);
		kernel_end = .;
	}
}