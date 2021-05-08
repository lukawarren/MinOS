#include "kernel.h"
#include "cpu/cpu.h"
#include "cpu/gdt.h"
#include "cpu/pic.h"
#include "stdout/uart.h"

extern uint32_t __tss_stack; // TSS stack from linker

extern "C" void kMain(multiboot_info_t* pMultibootInfo)
{
    // Init UART
    UART::Init();
    UART::WriteString("MinOS initialising...\n");

    // TSS
    CPU::TSS tss = CPU::CreateTSSEntry((uint32_t)&__tss_stack, 0x10);

    // Setup GDT, TSS, IDT and interrupts
    uint64_t GDTEntries[4] =
    {
        CPU::CreateGDTEntry(0,          0,          0),            // GDT entry at 0x0 cannot be used 
        CPU::CreateGDTEntry(0x00000000, 0xFFFFF,    GDT_CODE_PL0), // Code      - 0x8
        CPU::CreateGDTEntry(0x00000000, 0xFFFFF,    GDT_DATA_PL0), // Data      - 0x10
        CPU::CreateGDTEntry((uint32_t)&tss, sizeof(tss), TSS_PL0)
    };

    CPU::Init(GDTEntries, sizeof(GDTEntries) / sizeof(GDTEntries[0]), PIC_MASK_PIT_AND_KEYBOARD, PIC_MASK_ALL);

    // Enable interrupts
    CPU::EnableInterrupts();

    for (;;) asm("nop"); // Hang
}