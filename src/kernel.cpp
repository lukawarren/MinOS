#include "kernel.h"
#include "io/vga.h"
#include "io/uart.h"
#include "io/pic.h"
#include "io/io.h"
#include "io/pit.h"
#include "memory/gdt.h"
#include "memory/tss.h"
#include "memory/idt.h"
#include "memory/paging.h"
#include "interrupts/interrupts.h"
#include "interrupts/keyboard.h"
#include "interrupts/timer.h"
#include "cli.h"
#include "stdlib.h"

TSS tssEntry;
uint64_t GDTTable[4];
multiboot_info_t* pMultiboot;

extern "C" void kernel_main(multiboot_info_t* mbd) 
{
    pMultiboot = mbd;
    
    VGA_Clear();
    VGA_EnableCursor();

    // Welcome message
    VGA_printf("---------------------------------------------------------------------------------", false, VGA_COLOUR_GREEN);
    VGA_printf("                                      MinOS                                      ", false, VGA_COLOUR_GREEN);
    VGA_printf("--------------------------------------------------------------------------------", false, VGA_COLOUR_GREEN);
    VGA_printf(" ");

    // Start COM1 serial port
    UART COM1 = UART(UART::COM1);
    UART::pCOM = &COM1;
    COM1.printf("MinOS running from COM1 at ", false);
    COM1.printf<uint16_t, true>((uint16_t)UART::COM1);
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("UART communication established on COM1 at ", false);
    VGA_printf<uint16_t, true>((uint16_t)COM1.m_Com);

    // Create TSS
    tssEntry = CreateTSSEntry(0x0);

    // Construct GDT entries
    GDTTable[0] = CreateGDTEntry(0, 0, 0); // GDT entry at 0x0 cannot be used
    GDTTable[1] = CreateGDTEntry(0x00000000, 0xffffffff, GDT_CODE_PL0); // Code - 0x8
    GDTTable[2] = CreateGDTEntry(0x00000000, 0xffffffff, GDT_DATA_PL0); // Data - 0x10
    GDTTable[3] = CreateGDTEntry((uint32_t) &tssEntry, sizeof(tssEntry), TSS_PL0);  // TSS, 0x18

    // Load GDT
    LoadGDT(GDTTable, sizeof(GDTTable));
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("GDT sucessfully loaded");

    // Load TSS
    LoadTSS((uint32_t)&GDTTable[3] - (uint32_t)&GDTTable);
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("TSS sucessfully loaded");

    // Create keyboard
    CLI cli = CLI(OnCommand);
    Keyboard keyboard(&cli);

    // Setup PIT
    InitPIT();

    // Init PIC, create IDT entries and enable interrupts
    InitInterrupts(PIC_MASK_PIT_AND_KEYBOARD, PIC_MASK_ALL, &keyboard);
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("IDT sucessfully loaded");

    // Read memory map from GRUB
    if ((mbd->flags & 6) == 0) {  VGA_printf("[Failure] Multiboot error!", true, VGA_COLOUR_LIGHT_RED); }

    /*
        Let's use the second block of extended memory,
        which extends from 0x1000000 (16mb) to just before memory mapped PCI devices (if any)
    */
    multiboot_memory_map_t* entry = (multiboot_memory_map_t *)(mbd->mmap_addr);
    uint32_t maxMemoryRange = 0;
    while ((multiboot_uint32_t) entry < mbd->mmap_addr + mbd->mmap_length)
    {
        if (entry->addr == 0x100000)
        {
            VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
            VGA_printf("Extended memory block detected with length ", false);
            VGA_printf<uint64_t, true>(entry->len, false);
            VGA_printf(" (", false);
            VGA_printf((uint32_t)(entry->len / 1024 / 1024), false);
            VGA_printf(" MB)");

            maxMemoryRange = (uint32_t)entry->addr + (uint32_t)entry->len;
        }
        entry = (multiboot_memory_map_t *) ((unsigned int) entry + entry->size + sizeof(entry->size));
    }
    if (maxMemoryRange == 0)
    {
        VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_GREEN);
        VGA_printf("Memory mapping failed!");
    }

    // Page frame allocation
    InitPaging(maxMemoryRange);
    
    // Start prompt and hang
    VGA_printf("");
    keyboard.OnKeyUpdate('\0');

    // Hang and wait for interrupts
    while (true) { asm("hlt"); }
}

void OnCommand(char* buffer)
{
    #if DO_SOUND_DEMO
    if (strcmp(buffer, "$ help")) VGA_printf("Commands: gdt, multiboot, jingle", false);
    #else
    if (strcmp(buffer, "$ help")) VGA_printf("Commands: gdt, multiboot", false);
    #endif
    else if (strcmp(buffer, "$ gdt"))
    {
        VGA_printf("GDT loaded at address ", false);
        VGA_printf<size_t, true>((size_t)&GDTTable, false);
        VGA_printf(" with ", false);
        VGA_printf(sizeof(GDTTable) / sizeof(GDTTable[0]), false);
        VGA_printf(" entries:");
        for (size_t i = 0; i < sizeof(GDTTable) / sizeof(GDTTable[0]); ++i) VGA_printf<uint64_t, true>(GDTTable[i]);
    }
    else if (strcmp(buffer, "$ multiboot"))
    {
        VGA_printf("Multiboot header at ", false);
        VGA_printf<uint32_t, true>((uint32_t)pMultiboot);
        VGA_printf("");
        VGA_printf("Framebuffer information:");
        VGA_printf("Resolution: ", false); 
        VGA_printf(pMultiboot->framebuffer_width, false);
        VGA_printf("x", false);
        VGA_printf(pMultiboot->framebuffer_height, false);
        VGA_printf("x", false);
        VGA_printf(pMultiboot->framebuffer_pitch / pMultiboot->framebuffer_width * 8);
    }
    #if DO_SOUND_DEMO
    else if (strcmp(buffer, "$ jingle"))
    {
        EnablePCSpeaker();
    }
    #endif

    else VGA_printf("Command not found", false, VGA_COLOUR_LIGHT_RED);
}