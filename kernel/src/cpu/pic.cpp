#include "cpu/pic.h"
#include "cpu/cpu.h"

namespace PIC
{
    void Init(const uint8_t mask1, const uint8_t mask2)
    {
        // Tell PICS to init
        CPU::outb(PIC1_COMMAND, PIC_INIT | ICW1_ICW4);
        CPU::outb(PIC2_COMMAND, PIC_INIT | ICW1_ICW4);

        // Remap IRQs to beginning of avaiable range
        CPU::outb(PIC1_DATA, 0x20); // Set master's offset to 0x20
        CPU::outb(PIC2_DATA, 0x28); // Set slave's to 0x28

        // More configuration
        CPU::outb(PIC1_DATA, ICW1_INTERVAL4); // Tell master PIC it has slave
        CPU::outb(PIC2_DATA, ICW1_SINGLE); // Tell slave PIC its cascade entity
        CPU::outb(PIC1_DATA, ICW4_8086);
        CPU::outb(PIC2_DATA, ICW4_8086);

        // Set masks
        CPU::outb(PIC1_DATA, mask1);
        CPU::outb(PIC2_DATA, mask2);
    }

    void EndInterrupt(uint8_t irq)
    {
        if (irq >= 8) CPU::outb(PIC2_COMMAND, PIC_EOI);
        CPU::outb(PIC1_COMMAND, PIC_EOI);
    }

    void SetMask(uint8_t irq)
    {
        uint16_t port = PIC1_DATA;
        if (irq >= 8) { port = PIC2_DATA; irq -= 8; }
        uint8_t value = (uint8_t)(CPU::inb(port) | (1 << irq));
        CPU::outb(port, value);
    }

    void ClearMask(uint8_t irq)
    {
        uint16_t port = PIC1_DATA;
        if (irq >= 8) { port = PIC2_DATA; irq -= 8; }
        uint8_t value = (uint8_t)(CPU::inb(port) & ~(1 << irq));
        CPU::outb(port, value);
    }
}