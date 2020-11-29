#include "interrupts.h"
#include "timer.h"
#include "../memory/idt.h"
#include "../io/pic.h"
#include "../io/io.h"
#include "../gfx/vga.h"

static IDT idt[256];
static Keyboard* keyboard;

uint8_t currentIRQ;

void InitInterrupts(uint8_t mask1, uint8_t mask2, Keyboard* k)
{
    keyboard = k;

    // Init PIC with masks
    PIC_Init(mask1, mask2);

    constexpr int offset = 0x20; // PIC has an offset that must be accounted for

    // Create IDT entries
    for (size_t i = 0; i < 256; ++i) idt[i] = CreateIDTEntry((uint32_t) &IRQUnknown, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+0] = CreateIDTEntry((uint32_t) &IRQ0, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+8] = CreateIDTEntry((uint32_t) &IRQ8, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+1] = CreateIDTEntry((uint32_t) &IRQ1, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+9] = CreateIDTEntry((uint32_t) &IRQ9, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+2] = CreateIDTEntry((uint32_t) &IRQ2, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+10] = CreateIDTEntry((uint32_t) &IRQ10, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+3] = CreateIDTEntry((uint32_t) &IRQ3, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+11] = CreateIDTEntry((uint32_t) &IRQ11, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+4] = CreateIDTEntry((uint32_t) &IRQ4, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+12] = CreateIDTEntry((uint32_t) &IRQ12, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+5] = CreateIDTEntry((uint32_t) &IRQ5, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+13] = CreateIDTEntry((uint32_t) &IRQ13, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+6] = CreateIDTEntry((uint32_t) &IRQ6, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+14] = CreateIDTEntry((uint32_t) &IRQ14, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+7] = CreateIDTEntry((uint32_t) &IRQ7, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+15] = CreateIDTEntry((uint32_t) &IRQ15, 0x8, ENABLED_R0_INTERRUPT);

    // IDT descriptor
    IDTDescriptor descriptor = IDTDescriptor(idt);

    // Load IDT and enable interrupts
    LoadIDT(&descriptor);
}

void HandleInterrupts(uint8_t irq, uint8_t unknown)
{
    if (unknown)
    {
        return;
        VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED);
        VGA_printf("Interrupt with error occured! (code ", false);
        VGA_printf<uint8_t, true>(irq, false);
        VGA_printf(")");
        return;
    }

    switch (irq)
    {
        case 0x0:
            OnTimerInterrupt();
        break;

        case 0x1: // Keyboard
        {
            if (keyboard == nullptr) break;
            uint8_t scancode = inb(0x60);
            keyboard->OnKeyUpdate(scancode);
            break;
        }

        default:
            VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED);
            VGA_printf("Interrupt with code ", false);
            VGA_printf<uint8_t, true>(irq, false);
            VGA_printf(" unrecognised!");
        break;
    }

    PIC_EndInterrupt(irq);
}