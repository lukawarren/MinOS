#include "io/uart.h"
#include "cpu/cpu.h"
#include "cpu/tss.h"
#include "klib.h"

extern uint32_t _kernel_end; // From linker
extern "C" { void kmain(void); }

void kmain(void) 
{
    uart::init();
    println("Initialising...");
    
    // Create TSS
    cpu::TSS tss = cpu::create_tss((size_t)&_kernel_end, 0x10);

    // Create GDT
    cpu::GDT gdt[6] =
    {
        cpu::create_gdt_entry(0,            0,           0),            // GDT entry at 0x0 cannot be used
        cpu::create_gdt_entry(0x00000000,   0xFFFFF,     GDT_CODE_PL0), // Code      - 0x8
        cpu::create_gdt_entry(0x00000000,   0xFFFFF,     GDT_DATA_PL0), // Data      - 0x10
        cpu::create_gdt_entry(0x00000000,   0xFFFFF,     GDT_CODE_PL3), // User code - 0x18
        cpu::create_gdt_entry(0x00000000,   0xFFFFF,     GDT_DATA_PL3), // User data - 0x20
        cpu::create_gdt_entry((size_t)&tss, sizeof(tss), TSS_PL0)
    };

    // TSS descriptor - offset from start of GDT OR'ed with 3 to enable RPL 3
    const uint16_t tssDescriptor = (5 * sizeof(uint64_t)) | 3;

    // Load above to CPU
    cpu::load_gdt(gdt, sizeof(cpu::GDT) * sizeof(gdt) / sizeof(gdt[0]));

    while(1) {}
}