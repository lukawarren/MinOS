#include "kernel.h"
#include "cpu/cpu.h"
#include "cpu/gdt.h"
#include "cpu/pic.h"
#include "cpu/cmos.h"
#include "stdout/uart.h"
#include "memory/memory.h"
#include "memory/modules.h"
#include "multitask/multitask.h"
#include "multitask/elf.h"
#include "io/framebuffer.h"
#include "stdlib.h"

extern uint32_t __tss_stack; // TSS stack from linker

extern "C" void kMain(multiboot_info_t* pMultibootInfo)
{
    // Init UART
    UART::Init();
    UART::WriteString("MinOS initialising...\n");

    // Print time
    auto time = CMOS::GetTime();
    UART::WriteString("[CMOS] Time is ");
    UART::WriteNumber(time.hour);
    UART::WriteString(":");
    UART::WriteNumber(time.minute);
    UART::WriteString("\n");

    // TSS
    CPU::TSS tss = CPU::CreateTSSEntry((uint32_t)&__tss_stack, 0x10);

    // Setup GDT, TSS, IDT and interrupts
    uint64_t GDTEntries[6] =
    {
        CPU::CreateGDTEntry(0,          0,          0),            // GDT entry at 0x0 cannot be used 
        CPU::CreateGDTEntry(0x00000000, 0xFFFFF,    GDT_CODE_PL0), // Code      - 0x8
        CPU::CreateGDTEntry(0x00000000, 0xFFFFF,    GDT_DATA_PL0), // Data      - 0x10
        CPU::CreateGDTEntry(0x00000000, 0xFFFFF,    GDT_CODE_PL3), // User code - 0x18
        CPU::CreateGDTEntry(0x00000000, 0xFFFFF,    GDT_DATA_PL3), // User data - 0x20
        CPU::CreateGDTEntry((uint32_t)&tss, sizeof(tss), TSS_PL0)
    };

    // TSS descriptor - offset from start of GDT OR'ed with 3 to enable RPL 3
    const uint16_t tssDescriptor = (5 * sizeof(uint64_t)) | 3;

    CPU::Init(GDTEntries, sizeof(GDTEntries) / sizeof(GDTEntries[0]), tssDescriptor, PIC_MASK_PIT, PIC_MASK_ALL);

    // Check multiboot, quickly grab GRUB modules, then configure memory
    if ((pMultibootInfo->flags & 6) == false) UART::WriteString("Multiboot error!");
    Modules::Init(pMultibootInfo);
    Memory::Init(pMultibootInfo);

    // Sanity check it all and reserve memory before it's snatched again!
    Modules::PostInit();

    // Setup devices
    Framebuffer::Init(pMultibootInfo);

    // Setup tasks
    Multitask::Init();
    
    Multitask::CreateTask("Colonel", Multitask::TaskType::KERNEL, []
    {
        UART::WriteString("[Colonel] Hello!\n");
        while(1) asm("nop");
    });

    Multitask::CreateTask("Userland");

    Multitask::CreateTask("Colonel 2", Multitask::TaskType::KERNEL, []
    {
        UART::WriteString("[Colonel 2] I'm not even page faulting!\n");
        while(1) asm("nop");
    });

    // Enable interrupts
    CPU::EnableInterrupts();

    for (;;) asm("nop"); // Hang
}