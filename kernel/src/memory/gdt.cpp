#include "gdt.h"
#include "../gfx/vga.h"

void PrintGDT(const uint64_t* pTable, const unsigned int nEntries)
{
    VGA_printf("GDT loaded at ", false);
    VGA_printf<uint32_t, true>((uint32_t)pTable);
    VGA_printf("");

    VGA_printf("Base", false);
    VGA_column += 10;
    VGA_printf("Limit", false);
    VGA_column += 10;
    VGA_printf("DPL", false);
    VGA_column += 10;
    VGA_printf("Type", false);
    VGA_column += 14;
    VGA_printf("Selector");
    
    for (unsigned int i = 0; i < nEntries; ++i)
    {
        uint64_t descriptor = pTable[i];

        // Base adress
        uint32_t baseAddress = (uint32_t)(descriptor >> 16) & 0xFF; // Bits 0:15
        baseAddress |= (uint32_t)(descriptor >> 16) & 0xFF00;       // Bits 16:23
        baseAddress |= (uint32_t)(descriptor >> 16) & 0xFF0000;     // Bits 24:31
        VGA_printf<uint32_t, true, 8>(baseAddress, false);

        VGA_column += 4;

        // Limit
        uint32_t limit = (uint32_t)(descriptor & 0xFFFF);   // Bits 0:15
        limit |= (uint32_t)(descriptor >> 32) & 0xF0000;    // Bits 16:19
        VGA_printf<uint32_t, true, 8>(limit, false);

        VGA_column += 5;
        
        // Get access byte and flags
        uint32_t flags = (uint32_t)(descriptor >> 40) & 0xF0FF;

        // DPL
        uint8_t DPL = (flags & 0b01100000) >> 5;
        VGA_printf(DPL, false);

        VGA_column += 12;

        // Type 
        if (((flags >> 0x4) & 1) == (SEG_DESCTYPE(0)))
        {
            if (((flags >> 0x7) & 1) == (SEG_PRES(0))) VGA_printf("Unused", false);
            else VGA_printf("TSS", false);
        }
        else
        {
            if (flags & 0b1000) VGA_printf("32-bit code", false);
            else VGA_printf("32-bit data", false);
        }

        VGA_column = 60;

        // Selector
        VGA_printf<uint32_t, true>(i * 8);
    }
}