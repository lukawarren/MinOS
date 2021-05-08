#include "cpu/cpu.h"
#include "cpu/pic.h"
#include "interrupts/interrupts.h"

namespace CPU
{
    static IDT idt[256];

    void Init(const uint64_t* gdt, const uint32_t nEntries, const uint8_t mask1, const uint8_t mask2)
    {
        // Load GDT
        LoadGDT(gdt, sizeof(uint64_t)*nEntries);

        // Init PIC with masks
        PIC::Init(mask1, mask2);

        // Build empty IDT
        using namespace Interrupts;
        constexpr int offset = 0x20; // PIC has an offset that must be accounted for
        for (size_t i = 0; i < 256; ++i) idt[i] = CreateIDTEntry((uint32_t) &BlankIRQ, 0x8, ENABLED_R0_INTERRUPT);
        
        // Standard interrupts
        idt[offset+0]  = CreateIDTEntry((uint32_t) IRQ0,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+1]  = CreateIDTEntry((uint32_t) IRQ1,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+2]  = CreateIDTEntry((uint32_t) IRQ2,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+3]  = CreateIDTEntry((uint32_t) IRQ3,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+4]  = CreateIDTEntry((uint32_t) IRQ4,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+5]  = CreateIDTEntry((uint32_t) IRQ5,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+6]  = CreateIDTEntry((uint32_t) IRQ6,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+7]  = CreateIDTEntry((uint32_t) IRQ7,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+8]  = CreateIDTEntry((uint32_t) IRQ8,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+9]  = CreateIDTEntry((uint32_t) IRQ9,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+10] = CreateIDTEntry((uint32_t) IRQ10, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+11] = CreateIDTEntry((uint32_t) IRQ11, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+12] = CreateIDTEntry((uint32_t) IRQ12, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+13] = CreateIDTEntry((uint32_t) IRQ13, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+14] = CreateIDTEntry((uint32_t) IRQ14, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+15] = CreateIDTEntry((uint32_t) IRQ15, 0x8, ENABLED_R0_INTERRUPT);

        // Load IDT
        IDTDescriptor descriptor(idt);
        LoadIDT(&descriptor);
    }

    uint64_t CreateGDTEntry(const uint32_t base, const uint32_t limit, const uint16_t flag)
    {
        uint64_t descriptor = 0;
    
        // Create the high 32 bit segment
        descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
        descriptor |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
        descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
        descriptor |=  base        & 0xFF000000;         // set base bits 31:24
    
        // Shift by 32 to allow for low part of segment
        descriptor <<= 32;
    
        // Create the low 32 bit segment
        descriptor |= base  << 16;                       // set base bits 15:0
        descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0

        return descriptor;
    }

    TSS CreateTSSEntry(const uint32_t stackPointer0, const uint32_t dataSegmentDescriptor0)
    {
        TSS tss = {};
        tss.ss0 = dataSegmentDescriptor0;
        tss.esp0 = stackPointer0;
        tss.iomap_base = sizeof(TSS);
        return tss;
    }

    IDT CreateIDTEntry(const uint32_t entrypoint, const uint16_t selector, const uint8_t attributes)
    {
        IDT idtEntry = IDT();
        idtEntry.offsetLower = entrypoint & 0xFFFF;
        idtEntry.selector = selector;
        idtEntry.zero = 0;
        idtEntry.typeAttribute = attributes;
        idtEntry.offsetHigher = (uint16_t)((entrypoint & 0xFFFF0000) >> 16);
        return idtEntry;
    }

    void EnableInterrupts()
    {
        asm("sti");
    }
}