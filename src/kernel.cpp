#include "kernel.h"
#include "io/vga.h"
#include "io/uart.h"
#include "io/pic.h"
#include "io/io.h"
#include "memory/gdt.h"
#include "memory/tss.h"
#include "memory/idt.h"
#include "interrupts/interrupts.h"

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

    // Create TSS
    constexpr TSS tssEntry = CreateTSSEntry(0x0);

    // Construct GDT entries
    const uint64_t GDTTable[4] =
    {
        CreateGDTEntry(0, 0, 0), // GDT entry at 0x0 cannot be used
        CreateGDTEntry(0x00000000, 0xffffffff, GDT_CODE_PL0), // Code starting from 4MB, 4MB large - 0x8
        CreateGDTEntry(0x00000000, 0xffffffff, GDT_DATA_PL0), // Data starting from 8MB, 4MB large - 0x10
        CreateGDTEntry((uint32_t) &tssEntry, sizeof(tssEntry), TSS_PL0)  // TSS, 0x18
    };

    // Load GDT
    LoadGDT(GDTTable, sizeof(GDTTable));
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("GDT sucessfully loaded");

    // Load TSS
    LoadTSS((uint32_t)&GDTTable[3] - (uint32_t)&GDTTable);
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("TSS sucessfully loaded");

    // Init PIC, create IDT entries and enable interrupts
    InitInterrupts();
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("IDT sucessfully loaded");

    while (true) { asm("hlt"); }
}