#pragma once
#ifndef IDT_H
#define IDT_H

#include <stddef.h>
#include <stdint.h>

#include "../io/pic.h"
#include "stdlib.h"

/*
    0x00 Divide by zero
    0x01 Single step interrupt
    0x02 NMI
    0x03 Breakpoint (used by debuggers)
    0x04 Overflow
    0x05 Bounds
    0x06 Invalid opcode
    0x07 Coprocessor not available
    0x08 Double fault
    0x09 Coprocessor segment overrun (386 or earlier only)
    0x0A Invalid Task State Segment
    0x0B Segment not present
    0x0C Stack fault
    0x0D General protection fault
    0x0E Page fault
    0x0F reserved
    0x10 Math fault
    0x11 Alignment check
    0x12 Machine checkl
    0x13 SIMD floating point exception
    0x14 virtualisation exception
    0x15 control protection expression
*/

#define IDT_ENABLED(x)  ((x & 0b01) << 7)
#define MIN_PRIV(x)     ((x & 0b11) << 6)

#define GATE_INTERRUPT_32   0xE
#define GATE_TRAP_32        0xF

#define DISABLED_R0_INTERRUPT   IDT_ENABLED(0) | MIN_PRIV(0) | GATE_INTERRUPT_32
#define DISABLED_R3_INTERRUPT   IDT_ENABLED(0) | MIN_PRIV(3) | GATE_INTERRUPT_32
#define ENABLED_R0_INTERRUPT    IDT_ENABLED(1) | MIN_PRIV(0) | GATE_INTERRUPT_32
#define ENABLED_R3_INTERRUPT    IDT_ENABLED(1) | MIN_PRIV(3) | GATE_INTERRUPT_32

struct IDT
{
    uint16_t offsetLower = 0;
    uint16_t selector = 0;
    uint8_t zero = 0;
    uint8_t typeAttribute = 0;
    uint16_t offsetHigher = 0;
} __attribute__((packed));

inline constexpr IDT CreateIDTEntry(uint32_t entrypoint, uint16_t selector, uint8_t attributes)
{
    IDT idt = IDT();
    idt.offsetLower = entrypoint & 0xFFFF;
    idt.selector = selector;
    idt.zero = 0;
    idt.typeAttribute = attributes;
    idt.offsetHigher = (uint16_t)((entrypoint & 0xFFFF0000) >> 16);
    return idt;
}

struct IDTDescriptor
{
    uint16_t idtLength;
    uint32_t idtAddress;

    IDTDescriptor(const IDT* address)
    {
        idtAddress = (uint32_t) address;
        idtLength = sizeof(IDT) * 256 - 1;
    }
} __attribute__((packed));

extern "C"
{
    extern void LoadIDT(const IDTDescriptor* IDTDescriptor);
    void HandleInterrupts(uint32_t irq, uint32_t unknown);
    void HandleExceptions(uint32_t irq, uint32_t eip, uint32_t errorCode, Registers regs);
    void SanityCheck(uint32_t eip);

    extern void IRQ0();
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

    extern void IRQSyscall80();

    extern void IRQUnknown();
}

extern volatile bool bIRQShouldJump;
extern volatile uint32_t IRQReturnAddress;
extern volatile uint32_t IRQJumpEIP;

#endif