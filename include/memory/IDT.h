#pragma once
#ifndef IDT_H
#define IDT_H

#include "stddef.h"
#include "stdint.h"

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
#define ENABLED_R0_INTERRUPT    IDT_ENABLED(0) | MIN_PRIV(0) | GATE_INTERRUPT_32

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
    idt.typeAttribute = 0x8e;
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
}

#endif