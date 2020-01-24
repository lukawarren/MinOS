#include "gdt.h"

#include "../std/stdout.h"

uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
    uint64_t descriptor;
 
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

void initGDT()
{
    // Create TSS
    uint32_t TSS[16];

	// Create GDT
	uint64_t GDT[4];
	GDT[0] = create_descriptor(0, 0, 0); //	Full 4GB addresses untranslated				0x0
    GDT[1] = create_descriptor(0, 0xffffffff, (GDT_CODE_PL0)); //						0x8
    GDT[2] = create_descriptor(0, 0xffffffff, (GDT_DATA_PL0)); //						0x10
	GDT[3] = create_descriptor((uint32_t) &TSS, sizeof(TSS), 0x89); //					0x18
	
    printi("Loading GDT at base address ");
    printh((uint32_t) &GDT);
    printi(" with size ");
    printh((uint32_t) sizeof(GDT));
    printf("");

	setGDT(GDT, sizeof(GDT));
}