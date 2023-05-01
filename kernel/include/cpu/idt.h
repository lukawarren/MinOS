#pragma once
#include "klib.h"

#define IDT_ENABLED(x)  ((x & 0b01) << 7)
#define MIN_PRIV(x)     ((x & 0b11) << 5)

#define GATE_INTERRUPT_32   0xE
#define GATE_TRAP_32        0xF

#define DISABLED_R0_INTERRUPT   IDT_ENABLED(0) | MIN_PRIV(0) | GATE_INTERRUPT_32
#define DISABLED_R3_INTERRUPT   IDT_ENABLED(0) | MIN_PRIV(3) | GATE_INTERRUPT_32
#define ENABLED_R0_INTERRUPT    IDT_ENABLED(1) | MIN_PRIV(0) | GATE_INTERRUPT_32
#define ENABLED_R3_INTERRUPT    IDT_ENABLED(1) | MIN_PRIV(3) | GATE_INTERRUPT_32

namespace cpu
{
    struct IDT
    {
        uint16_t offsetLower = 0;
        uint16_t selector = 0;
        uint8_t  zero = 0;
        uint8_t  typeAttribute = 0;
        uint16_t offsetHigher = 0;
    } __attribute__((packed));

    struct IDTDescriptor
    {
        uint16_t idtLength;
        uint32_t idtAddress;

        IDTDescriptor(const IDT* address)
        {
            idtAddress = (uint32_t)address;
            idtLength = sizeof(IDT) * 256 - 1;
        }
    } __attribute__((packed));

    constexpr IDT create_idt_entry(const uint32_t entrypoint, const uint16_t selector, const uint8_t attributes)
    {
        IDT idtEntry = IDT {};
        idtEntry.offsetLower = entrypoint & 0xFFFF;
        idtEntry.selector = selector;
        idtEntry.zero = 0;
        idtEntry.typeAttribute = attributes;
        idtEntry.offsetHigher = (uint16_t)((entrypoint & 0xFFFF0000) >> 16);
        return idtEntry;
    }
}
