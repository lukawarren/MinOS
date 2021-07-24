#pragma once
#ifndef TSS_H
#define TSS_H

#include <stddef.h>
#include <stdint.h>

namespace CPU
{
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
}

#endif