#include "kernel.h"
#include "io/vga.h"
#include "io/uart.h"

extern "C" void kernel_main(void) 
{
    VGA_Clear();
    VGA_EnableCursor();

    // Welcome message
    VGA_printf("---------------------------------------------------------------------------------", false, VGA_COLOUR_GREEN);
    VGA_printf("                                      MinOS                                      ", false, VGA_COLOUR_GREEN);
    VGA_printf("--------------------------------------------------------------------------------", false, VGA_COLOUR_GREEN);
    VGA_printf(" ");

    // Start COM1 serial port
    UART COM1 = UART(UART::COM1);
    COM1.printf("MinOS running from COM1 at ", false);
    COM1.printf<uint16_t, true>((uint16_t)UART::COM1);
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("UART communication established on COM1 at ", false);
    VGA_printf<uint16_t, true>((uint16_t)COM1.m_Com);
}