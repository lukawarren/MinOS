#pragma once
#ifndef TSS_H
#define TSS_H

#include <stddef.h>
#include <stdint.h>

struct TSS
{
    uint32_t previousTSS = 0;   // The previous TSS, forming a linked list
    uint32_t esp0 = 0;          // The stack pointer to load for kernel mode
    uint32_t ss0 = 0;           // The stack segment to load for kernel mode
    uint32_t esp1 = 0;
    uint32_t ss1 = 0;
    uint32_t esp2 = 0;
    uint32_t ss2 = 0;
    uint32_t cr3 = 0;
    uint32_t eip = 0;
    uint32_t eflags = 0;
    uint32_t eax = 0;
    uint32_t ecx = 0;
    uint32_t edx = 0;
    uint32_t ebx = 0;
    uint32_t esp = 0;
    uint32_t ebp = 0;
    uint32_t esi = 0;
    uint32_t edi = 0;
    uint32_t es = 0;         
    uint32_t cs = 0;        
    uint32_t ss = 0;        
    uint32_t ds = 0;        
    uint32_t fs = 0;       
    uint32_t gs = 0;         
    uint32_t ldt = 0;      
    uint16_t trap = 0;
    uint16_t iomap_base = 0;
} __attribute__((packed));

inline constexpr TSS CreateTSSEntry(const uint32_t stackPointer0, const uint32_t dataSegmentDescriptor0)
{
    TSS tss = {};
    tss.ss0 = dataSegmentDescriptor0;
    tss.esp0 = stackPointer0;
    tss.iomap_base = sizeof(TSS);

    tss.cs   = 0x0b;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;
    return tss;
}

extern "C"
{
    extern void LoadTSS(const uint16_t descriptor);
}

#endif