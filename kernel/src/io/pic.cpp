#include "pic.h"

#include "io.h"
#include "../memory/idt.h"

void PIC_Init(uint8_t mask1, uint8_t mask2)
{
    // Tell PICS to init
    outb(PIC1_COMMAND, PIC_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, PIC_INIT | ICW1_ICW4);

    // Remap IRQs to beginning of avaiable range
    outb(PIC1_DATA, 0x20); // Set master's offset to 0x20
    outb(PIC2_DATA, 0x28); // Set slave's to 0x28

    // More configuration
    outb(PIC1_DATA, ICW1_INTERVAL4); // Tell master PIC it has slave
    outb(PIC2_DATA, ICW1_SINGLE); // Tell slave PIC its cascade entity
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // Set masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void PIC_EndInterrupt(uint8_t irq)
{
    if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_SetMask(uint8_t irq)
{
    uint16_t port = PIC1_DATA;
    if (irq >= 8) { port = PIC2_DATA; irq -= 8; }
    uint8_t value = (uint8_t)(inb(port) | (1 << irq));
    outb(port, value);
}

void PIC_ClearMask(uint8_t irq)
{
    uint16_t port = PIC1_DATA;
    if (irq >= 8) { port = PIC2_DATA; irq -= 8; }
    uint8_t value = (uint8_t)(inb(port) & ~(1 << irq));
    outb(port, value);
}