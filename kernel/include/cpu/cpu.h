#pragma once
#include "gdt.h"
#include "idt.h"
#include "tss.h"

namespace cpu
{
    extern IDT idt[256];
    extern GDT gdt[6];
    extern TSS tss;

    void init();

    struct Registers
    {
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t esp;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;
    };

    extern "C"
    {
        void load_gdt(const GDT* gdt, const size_t size);
        void load_tss(const uint16_t descriptor);
        void load_idt(const IDTDescriptor* descriptor);
        void flush_tlb();
        void enable_paging();
        void enable_fpu();
    }

    inline void enable_interrupts()
    {
        asm volatile("sti");
    }

    inline void outb(uint16_t port, uint8_t data)
    {
        asm volatile( "outb %0, %1" : : "a"(data), "d"(port));
    }

    inline uint8_t inb(uint16_t port)
    {
        uint8_t ret;
        asm volatile("inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port));
        return ret;
    }

    inline void outw(uint16_t port, uint16_t data)
    {
        asm volatile("out %0, %1" : : "a"(data), "d"(port));
    }

    inline uint16_t inw(uint16_t port)
    {
        uint16_t ret;
        asm volatile("in %1, %0"
                    : "=a"(ret)
                    : "Nd"(port));
        return ret;
    }

    inline void outl(uint16_t port, uint32_t data)
    {
        asm volatile("outl %0, %1" : : "a"(data), "d"(port));
    }

    inline uint32_t inl(uint16_t port)
    {
        uint32_t ret;
        asm volatile("inl %1, %0"
                    : "=a"(ret)
                    : "Nd"(port));
        return ret;
    }

    inline void set_cr3(size_t cr3)
    {
        asm volatile("mov %0, %%cr3" :: "r"(cr3));
    }
}