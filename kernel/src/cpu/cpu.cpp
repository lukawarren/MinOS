#include "cpu/cpu.h"

namespace CPU{

void Init(const uint64_t* gdt, const uint32_t nEntries)
{
    // Load GDT
    LoadGDT(gdt, sizeof(uint64_t)*nEntries);
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
}