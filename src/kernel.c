#include "std/stdout.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "smbios/sysinfo.h"
#include "keyboard/keyboard.h"

void kernel_main(void) 
{
	// Initialize terminal interface
	stdout_initialize();
	
	// Banner
	stdout_setcolor(VGA_COLOR_WHITE);
	printi("------------------------------------------------------------------------------ ");
	printi("                                    Min OS                                     ");
	printi("------------------------------------------------------------------------------ ");
	printf("");

	// Init sequence
	stdout_setcolor(VGA_COLOR_LIGHT_GREEN);
	printf("Initialising kernel...");

	initSMBIOS();
	printSMBIOS();

	initGDT();
	initIDT();

	// And... relax
	printf("");
	stdout_setcolor(VGA_COLOR_WHITE);
	initKeyboard();

	// Hang
	for (;;)
	{
		__asm__("hlt");
	}

	stdout_setcolor(VGA_COLOR_LIGHT_RED);
	printf("Ok so we appear to be hanging...");
	printf("Well this is going to get boring...");
}