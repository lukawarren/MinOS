#include "kernel.h"
#include "gfx/vga.h"
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
#include "multitask/taskSwitch.h"
#include "multitask/multitask.h"
#include "multitask/modules.h"
#include "multitask/ring.h"
#include "cli.h"
#include "stdlib.h"

TSS tssEntry;
uint64_t GDTTable[6];
multiboot_info_t* pMultiboot;

Task* task1;
Task* task2;
Task* task3;
void Process1();
void Process2();
void Process3();

int barProgress[3];
const VGA_Colour barColours[3] = { VGA_COLOUR_LIGHT_YELLOW, VGA_COLOUR_LIGHT_GREEN, VGA_COLOUR_LIGHT_RED };
void DrawBar(int processID);

extern "C" void kernel_main(multiboot_info_t* mbd) 
{
    pMultiboot = mbd;

    VGA_Init({  pMultiboot->framebuffer_width, pMultiboot->framebuffer_height, 
                pMultiboot->framebuffer_pitch, (uint32_t*)pMultiboot->framebuffer_addr });
    VGA_Clear();

    // Welcome message
    VGA_printf("-------------------------------------------------------------------------------", true, VGA_COLOUR_GREEN);
    VGA_printf("                                    MinOS                                      ", true, VGA_COLOUR_GREEN);
    VGA_printf("-------------------------------------------------------------------------------", true, VGA_COLOUR_GREEN);
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
    tssEntry = CreateTSSEntry(0x0, 0x10); // Stack pointer (zero for now, not needed yet) and ring 0 data selector 

    // Construct GDT entries (0xFFFFF actually translates to all of memory)
    GDTTable[0] = CreateGDTEntry(0, 0, 0);                                          // GDT entry at 0x0 cannot be used
    GDTTable[1] = CreateGDTEntry(0x00000000, 0xFFFFF, GDT_CODE_PL0);                // Code      - 0x8
    GDTTable[2] = CreateGDTEntry(0x00000000, 0xFFFFF, GDT_DATA_PL0);                // Data      - 0x10
    GDTTable[3] = CreateGDTEntry(0x00000000, 0xFFFFF, GDT_CODE_PL3);                // User code - 0x18
    GDTTable[4] = CreateGDTEntry(0x00000000, 0xFFFFF, GDT_DATA_PL3);                // User data - 0x20
    GDTTable[5] = CreateGDTEntry((uint32_t) &tssEntry, sizeof(tssEntry), TSS_PL0);  // TSS       - 0x28

    // Load GDT
    LoadGDT(GDTTable, sizeof(GDTTable));
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("GDT sucessfully loaded");

    // Load TSS
    LoadTSS(((uint32_t)&GDTTable[5] - (uint32_t)&GDTTable) | 0b11); // Set last 2 bits for RPL 3
    SetTSSForMultitasking(&tssEntry);
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("TSS sucessfully loaded");

    // Read memory map from GRUB
    if ((mbd->flags & 6) == 0) {  VGA_printf("[Failure] Multiboot error!", true, VGA_COLOUR_LIGHT_RED); }

    // Map out memory
    uint32_t maxMemoryRange = GetMaxMemoryRange(pMultiboot);

    // Pging messes up GRUB modules
    MoveGrubModules(pMultiboot);

    // Page frame allocation
    InitPaging(maxMemoryRange);

    // Create keyboard
    CLI cli = CLI(OnCommand);
    Keyboard keyboard(&cli);

    // Setup PIT
    InitPIT();
    
    // Init PIC, create IDT entries and enable interrupts
    InitInterrupts(PIC_MASK_PIT_AND_KEYBOARD, PIC_MASK_ALL, &keyboard);
    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("IDT sucessfully loaded");
    
    // Multiprocessing test
    VGA_printf("");
    task1 = CreateTask("Bar1", (uint32_t) &Process1);
    task2 = CreateTask("Bar2", (uint32_t) &Process2);
    task3 = CreateTask("Bar2", (uint32_t) &Process3);
    VGA_printf("");

    // Load GRUB modules
    LoadGrubModules(pMultiboot);

    // Start prompt
    keyboard.OnKeyUpdate('\0');
    keyboard.OnKeyUpdate('\0');

    EnableScheduler();

    // Hang and wait for interrupts
    while (true) { asm("hlt"); }
}

void OnCommand(char* buffer)
{
    #if DO_SOUND_DEMO
    if (strcmp(buffer, "$ help")) VGA_printf("Commands: gdt, paging, jingle", false);
    #else
    if (strcmp(buffer, "$ help")) VGA_printf("Commands: gdt, paging", false);
    #endif
    else if (strcmp(buffer, "$ gdt"))
    {
        PrintGDT(GDTTable, sizeof(GDTTable) / sizeof(GDTTable[0]));
    }
    else if (strcmp(buffer, "$ paging"))
    {
        PrintPaging();
    }
    #if DO_SOUND_DEMO
    else if (strcmp(buffer, "$ jingle"))
    {
        EnablePCSpeaker();
    }
    #endif

    else VGA_printf("Command not found", false, VGA_COLOUR_LIGHT_RED);
}

void DrawBar(int processID)
{
    uint32_t barHeight = 10;
    uint32_t barWidth = barProgress[processID]++ / 1;
    if (barWidth >= VGA_framebuffer.width) barWidth = VGA_framebuffer.width;

    for (uint32_t x = 0; x < barWidth; ++x)
        for (uint32_t y = VGA_framebuffer.height - 1 - barHeight*(processID+1); y < VGA_framebuffer.height - 1 - processID*barHeight; ++y)
            VGA_PutPixel(x, y, barColours[processID]);
}

void Process1() { while (true) DrawBar(0); }
void Process2() { while (true) DrawBar(1); }
void Process3() { while (true) DrawBar(2); }