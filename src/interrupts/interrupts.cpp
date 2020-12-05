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
    idt[offset+0] = CreateIDTEntry((uint32_t) IRQ0, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+8] = CreateIDTEntry((uint32_t) IRQ8, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+1] = CreateIDTEntry((uint32_t) IRQ1, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+9] = CreateIDTEntry((uint32_t) IRQ9, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+2] = CreateIDTEntry((uint32_t) IRQ2, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+10] = CreateIDTEntry((uint32_t) IRQ10, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+3] = CreateIDTEntry((uint32_t) IRQ3, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+11] = CreateIDTEntry((uint32_t) IRQ11, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+4] = CreateIDTEntry((uint32_t) IRQ4, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+12] = CreateIDTEntry((uint32_t) IRQ12, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+5] = CreateIDTEntry((uint32_t) IRQ5, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+13] = CreateIDTEntry((uint32_t) IRQ13, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+6] = CreateIDTEntry((uint32_t) IRQ6, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+14] = CreateIDTEntry((uint32_t) IRQ14, 0x8, ENABLED_R0_INTERRUPT);
    idt[offset+7] = CreateIDTEntry((uint32_t) IRQ7, 0x8, ENABLED_R0_INTERRUPT);   idt[offset+15] = CreateIDTEntry((uint32_t) IRQ15, 0x8, ENABLED_R0_INTERRUPT);

    // Exceptions
    idt[0] =    CreateIDTEntry((uint32_t) IRQException0, 0x8, ENABLED_R0_INTERRUPT);    idt[15] = CreateIDTEntry((uint32_t) IRQException15, 0x8, ENABLED_R0_INTERRUPT);
    idt[1] =    CreateIDTEntry((uint32_t) IRQException1, 0x8, ENABLED_R0_INTERRUPT);    idt[16] = CreateIDTEntry((uint32_t) IRQException16, 0x8, ENABLED_R0_INTERRUPT);
    idt[2] =    CreateIDTEntry((uint32_t) IRQException2, 0x8, ENABLED_R0_INTERRUPT);    idt[17] = CreateIDTEntry((uint32_t) IRQException17, 0x8, ENABLED_R0_INTERRUPT);
    idt[3] =    CreateIDTEntry((uint32_t) IRQException3, 0x8, ENABLED_R0_INTERRUPT);    idt[18] = CreateIDTEntry((uint32_t) IRQException18, 0x8, ENABLED_R0_INTERRUPT);
    idt[4] =    CreateIDTEntry((uint32_t) IRQException4, 0x8, ENABLED_R0_INTERRUPT);    idt[19] = CreateIDTEntry((uint32_t) IRQException19, 0x8, ENABLED_R0_INTERRUPT);
    idt[5] =    CreateIDTEntry((uint32_t) IRQException5, 0x8, ENABLED_R0_INTERRUPT);    idt[20] = CreateIDTEntry((uint32_t) IRQException20, 0x8, ENABLED_R0_INTERRUPT);
    idt[6] =    CreateIDTEntry((uint32_t) IRQException6, 0x8, ENABLED_R0_INTERRUPT);    idt[21] = CreateIDTEntry((uint32_t) IRQException21, 0x8, ENABLED_R0_INTERRUPT);
    idt[7] =    CreateIDTEntry((uint32_t) IRQException7, 0x8, ENABLED_R0_INTERRUPT);    idt[22] = CreateIDTEntry((uint32_t) IRQException22, 0x8, ENABLED_R0_INTERRUPT);
    idt[8] =    CreateIDTEntry((uint32_t) IRQException8, 0x8, ENABLED_R0_INTERRUPT);    idt[23] = CreateIDTEntry((uint32_t) IRQException23, 0x8, ENABLED_R0_INTERRUPT);
    idt[9] =    CreateIDTEntry((uint32_t) IRQException9, 0x8, ENABLED_R0_INTERRUPT);    idt[24] = CreateIDTEntry((uint32_t) IRQException24, 0x8, ENABLED_R0_INTERRUPT);
    idt[10] =   CreateIDTEntry((uint32_t) IRQException10, 0x8, ENABLED_R0_INTERRUPT);   idt[25] = CreateIDTEntry((uint32_t) IRQException25, 0x8, ENABLED_R0_INTERRUPT);
    idt[11] =   CreateIDTEntry((uint32_t) IRQException11, 0x8, ENABLED_R0_INTERRUPT);   idt[26] = CreateIDTEntry((uint32_t) IRQException26, 0x8, ENABLED_R0_INTERRUPT);
    idt[12] =   CreateIDTEntry((uint32_t) IRQException12, 0x8, ENABLED_R0_INTERRUPT);   idt[27] = CreateIDTEntry((uint32_t) IRQException27, 0x8, ENABLED_R0_INTERRUPT);
    idt[13] =   CreateIDTEntry((uint32_t) IRQException13, 0x8, ENABLED_R0_INTERRUPT);   idt[28] = CreateIDTEntry((uint32_t) IRQException28, 0x8, ENABLED_R0_INTERRUPT);
    idt[14] =   CreateIDTEntry((uint32_t) IRQException14, 0x8, ENABLED_R0_INTERRUPT);   idt[29] = CreateIDTEntry((uint32_t) IRQException29, 0x8, ENABLED_R0_INTERRUPT);
    idt[30] =   CreateIDTEntry((uint32_t) IRQException30, 0x8, ENABLED_R0_INTERRUPT);

    // Syscals
    idt[0x80] = CreateIDTEntry((uint32_t) IRQSyscall80, 0x8, ENABLED_R0_INTERRUPT);

    // IDT descriptor
    IDTDescriptor descriptor = IDTDescriptor(idt);

    // Load IDT and enable interrupts
    LoadIDT(&descriptor);
}

void HandleInterrupts(uint32_t irq, uint32_t unknown)
{
    if (unknown)
    {
        VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED);
        VGA_printf("Unknown interrupt occured! Implement proper exception IRQ?");
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
            VGA_printf<uint32_t, true>(irq, false);
            VGA_printf(" unrecognised!");
            while (true) asm("hlt");
        break;
    }

    PIC_EndInterrupt((uint8_t)irq);
}

void HandleExceptions(uint32_t irq, uint32_t eip, uint32_t errorCode)
{
    VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED);
    VGA_printf("Excecption ", false);
    VGA_printf<uint32_t, true>(irq, false);
    VGA_printf(" occured with error code ", false);
    VGA_printf<uint32_t, true>(errorCode, false);
    VGA_printf(" - eip: ", false);
    VGA_printf<uint32_t, true>(eip);

    while (true) asm("hlt");

    PIC_EndInterrupt((uint8_t)irq);
}

void HandleSyscalls(uint32_t code, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t edi, uint32_t esi, uint32_t ebp)
{
    VGA_printf("Syscall!");

    PIC_EndInterrupt(0x80);
}