ENTRY(_start)
STARTUP(../../../lib/i686-minos/lib/crt0.o)

SECTIONS
{
	/* Begin at 1GB */
	. = 0x40000000;
 
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
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