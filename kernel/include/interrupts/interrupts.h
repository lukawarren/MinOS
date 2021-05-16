#pragma once
#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>
#include <stddef.h>

#include "cpu/idt.h"

namespace Interrupts
{
    struct StackFrameRegisters
    {
        uint32_t gs;
        uint32_t fs;
        uint32_t es;
        uint32_t ds;
        uint32_t cr3;
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t esp;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;
    } __attribute__((packed));

    void Init(CPU::IDT* idt);

    extern "C"
    {
        void OnInterrupt(const uint32_t irq, const StackFrameRegisters registers);
        void OnException(const uint32_t irq, const StackFrameRegisters registers);

        extern void BlankIRQ();
        
        extern void IRQ1(); 
        extern void IRQ2();
        extern void IRQ3();
        extern void IRQ4();
        extern void IRQ5();
        extern void IRQ6();
        extern void IRQ7();
        extern void IRQ8();
        extern void IRQ9();
        extern void IRQ10();
        extern void IRQ11();
        extern void IRQ12();
        extern void IRQ13();
        extern void IRQ14();
        extern void IRQ15();

        extern void IRQException0();
        extern void IRQException1();
        extern void IRQException2();
        extern void IRQException3();
        extern void IRQException4();
        extern void IRQException5();
        extern void IRQException6();
        extern void IRQException7();
        extern void IRQException8();
        extern void IRQException9();
        extern void IRQException10();
        extern void IRQException11();
        extern void IRQException12();
        extern void IRQException13();
        extern void IRQException14();
        extern void IRQException15();
        extern void IRQException16();
        extern void IRQException17();
        extern void IRQException18();
        extern void IRQException19();
        extern void IRQException20();
        extern void IRQException21();
        extern void IRQException22();
        extern void IRQException23();
        extern void IRQException24();
        extern void IRQException25();
        extern void IRQException26();
        extern void IRQException27();
        extern void IRQException28();
        extern void IRQException29();
        extern void IRQException30();
    }
}

#endif