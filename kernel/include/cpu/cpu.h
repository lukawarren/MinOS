#pragma once
#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stddef.h>

#include "cpu/tss.h"

namespace CPU
{
    void Init(const uint64_t* gdt, const uint32_t nEntries);

    uint64_t CreateGDTEntry(const uint32_t base, const uint32_t limit, const uint16_t flag); // Could really be made constexpr
    TSS CreateTSSEntry(const uint32_t stackPointer0, const uint32_t dataSegmentDescriptor0);

    inline void outb(uint16_t port, uint8_t data)
    {
        asm volatile ( "outb %0, %1" : : "a"(data), "d"(port) );
    }

    inline uint8_t inb(uint16_t port)
    {
        uint8_t ret;
        asm volatile ( "inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
        return ret;
    }

    extern "C"
    {
        extern void LoadGDT(const uint64_t* gdt, const size_t size);
    }
}

#endif