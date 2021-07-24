#pragma once
#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stddef.h>

#include "cpu/tss.h"
#include "cpu/idt.h"

namespace CPU
{
    void Init(const uint64_t* gdt, const uint32_t nEntries, const uint16_t tssDescriptor, const uint8_t mask1, const uint8_t mask2);
    void EnableInterrupts();

    uint64_t CreateGDTEntry(const uint32_t base, const uint32_t limit, const uint16_t flag); // Could really be made constexpr
    TSS CreateTSSEntry(const uint32_t stackPointer0, const uint32_t dataSegmentDescriptor0);
    IDT CreateIDTEntry(const uint32_t entrypoint, const uint16_t selector, const uint8_t attributes);

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

    inline void outw(uint16_t port, uint16_t data)
    {
        asm volatile ( "out %0, %1" : : "a"(data), "d"(port) );
    }

    inline uint16_t inw(uint16_t port)
    {
        uint16_t ret;
        asm volatile ( "in %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
        return ret;
    }

    inline void outl(uint16_t port, uint32_t data)
    {
        asm volatile ( "outl %0, %1" : : "a"(data), "d"(port) );
    }

    inline uint32_t inl(uint16_t port)
    {
        uint32_t ret;
        asm volatile ( "inl %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
        return ret;
    }

    extern "C"
    {
        extern void LoadGDT(const uint64_t* gdt, const size_t size);
        extern void LoadIDT(const IDTDescriptor* descriptor);
        extern void LoadTSS(const uint16_t descriptor);

        extern void FlushTLB();
        extern void EnablePaging();
        extern void LoadPageDirectories(const uint32_t pageDirectoryAddress);

        extern uint32_t GetCR3();
    }
}

#endif