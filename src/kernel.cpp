#include "kernel.h"
#include "io/vga.h"

extern "C" void kernel_main(void) 
{
    VGA_Clear();

    VGA_printf("--------------------------------------------------------------------------------");
    VGA_printf("                                      MinOS                                     ");
    VGA_printf("-------------------------------------------------------------------------------");

    VGA_printf("Welcome to MinOS!");
    VGA_printf("COM1 Address port: ", false);
    VGA_printf<uint16_t, true>(*((uint16_t*)0x0410), false);

    VGA_DisableCursor();
    VGA_EnableCursor();
}