#include "cpu/cpu.h"
#include "cpu/tss.h"
#include "cpu/idt.h"
#include "klib.h"

extern uint32_t kernel_end; // From linker

namespace cpu
{
    IDT idt[256];
    GDT gdt[6];
    TSS tss;

    void init()
    {
        // Create TSS
        tss = create_tss((size_t)&kernel_end, 0x10);

        // Create GDT
        gdt[0] = create_gdt_entry(0,            0,           0);            // GDT entry at 0x0 cannot be used
        gdt[1] = create_gdt_entry(0x00000000,   0xFFFFF,     GDT_CODE_PL0); // Code      - 0x8
        gdt[2] = create_gdt_entry(0x00000000,   0xFFFFF,     GDT_DATA_PL0); // Data      - 0x10
        gdt[3] = create_gdt_entry(0x00000000,   0xFFFFF,     GDT_CODE_PL3); // User code - 0x18
        gdt[4] = create_gdt_entry(0x00000000,   0xFFFFF,     GDT_DATA_PL3); // User data - 0x20
        gdt[5] = create_gdt_entry((size_t)&tss, sizeof(tss), TSS_PL0);

        // TSS descriptor - offset from start of GDT OR'ed with 3 to enable RPL 3
        const uint16_t tss_descriptor = (5 * sizeof(uint64_t)) | 3;

        // Load above to CPU...
        load_gdt(gdt, sizeof(cpu::GDT) * sizeof(gdt) / sizeof(gdt[0]));
        load_tss(tss_descriptor);
        println("loaded GDT and TSS");

        // ...and interrupts too
        const IDTDescriptor descriptor(idt);
        load_idt(&descriptor);
        println("loaded IDT");

        // Setup FPU
        enable_fpu();
    }
}