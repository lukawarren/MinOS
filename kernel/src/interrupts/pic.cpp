#include "interrupts/pic.h"
#include "cpu/cpu.h"

namespace pic
{
    void init(const uint8_t mask1, const uint8_t mask2)
    {
        // Tell PICS to init
        cpu::outb(PIC1_COMMAND, PIC_INIT | ICW1_ICW4);
        cpu::outb(PIC2_COMMAND, PIC_INIT | ICW1_ICW4);

        // Remap IRQs to beginning of avaiable range
        cpu::outb(PIC1_DATA, 0x20); // Set master's offset to 0x20
        cpu::outb(PIC2_DATA, 0x28); // Set slave's to 0x28

        // More configuration
        cpu::outb(PIC1_DATA, ICW1_INTERVAL4); // Tell master PIC it has slave
        cpu::outb(PIC2_DATA, ICW1_SINGLE); // Tell slave PIC its cascade entity
        cpu::outb(PIC1_DATA, ICW4_8086);
        cpu::outb(PIC2_DATA, ICW4_8086);

        // Set masks
        cpu::outb(PIC1_DATA, mask1);
        cpu::outb(PIC2_DATA, mask2);
    }

    void end_interrupt(const uint8_t irq)
    {
        if (irq >= 8) cpu::outb(PIC2_COMMAND, PIC_EOI);
        cpu::outb(PIC1_COMMAND, PIC_EOI);
    }
}