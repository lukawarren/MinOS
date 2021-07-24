#pragma once
#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stddef.h>

#define IDT_ENABLED(x)  ((x & 0b01) << 7)
#define MIN_PRIV(x)     ((x & 0b11) << 5)

#define GATE_INTERRUPT_32   0xE
#define GATE_TRAP_32        0xF

#define DISABLED_R0_INTERRUPT   IDT_ENABLED(0) | MIN_PRIV(0) | GATE_INTERRUPT_32
#define DISABLED_R3_INTERRUPT   IDT_ENABLED(0) | MIN_PRIV(3) | GATE_INTERRUPT_32
#define ENABLED_R0_INTERRUPT    IDT_ENABLED(1) | MIN_PRIV(0) | GATE_INTERRUPT_32
#define ENABLED_R3_INTERRUPT    IDT_ENABLED(1) | MIN_PRIV(3) | GATE_INTERRUPT_32

namespace CPU
{
    struct IDT
    {
        uint16_t offsetLower = 0;
        uint16_t selector = 0;
        uint8_t zero = 0;
        uint8_t typeAttribute = 0;
        uint16_t offsetHigher = 0;
    } __attribute__((packed));

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
}

#endif