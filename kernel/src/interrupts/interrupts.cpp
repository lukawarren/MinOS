#include "interrupts/interrupts.h"
#include "multitask/multitask.h"
#include "multitask/syscalls.h"
#include "io/framebuffer.h"
#include "stdout/uart.h"
#include "cpu/pic.h"
#include "cpu/cpu.h"
#include "cpu/pit.h"
#include "stdlib.h"

namespace Interrupts
{
    void Init(CPU::IDT* idt)
    {
        using namespace CPU;

        // Build empty IDT
        constexpr int offset = 0x20; // PIC has an offset that must be accounted for
        for (size_t i = 0; i < 256; ++i) idt[i] = CreateIDTEntry((uint32_t) &BlankIRQ, 0x8, ENABLED_R0_INTERRUPT);
        
        // Standard interrupts (except PIT)
        idt[offset+1]  = CreateIDTEntry((uint32_t) IRQ1,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+2]  = CreateIDTEntry((uint32_t) IRQ2,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+3]  = CreateIDTEntry((uint32_t) IRQ3,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+4]  = CreateIDTEntry((uint32_t) IRQ4,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+5]  = CreateIDTEntry((uint32_t) IRQ5,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+6]  = CreateIDTEntry((uint32_t) IRQ6,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+7]  = CreateIDTEntry((uint32_t) IRQ7,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+8]  = CreateIDTEntry((uint32_t) IRQ8,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+9]  = CreateIDTEntry((uint32_t) IRQ9,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+10] = CreateIDTEntry((uint32_t) IRQ10, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+11] = CreateIDTEntry((uint32_t) IRQ11, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+12] = CreateIDTEntry((uint32_t) IRQ12, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+13] = CreateIDTEntry((uint32_t) IRQ13, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+14] = CreateIDTEntry((uint32_t) IRQ14, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+15] = CreateIDTEntry((uint32_t) IRQ15, 0x8, ENABLED_R0_INTERRUPT);

        // Exceptions
        idt[0]   = CreateIDTEntry((uint32_t) IRQException0,   0x8, ENABLED_R0_INTERRUPT);
        idt[1]   = CreateIDTEntry((uint32_t) IRQException1,   0x8, ENABLED_R0_INTERRUPT);
        idt[2]   = CreateIDTEntry((uint32_t) IRQException2,   0x8, ENABLED_R0_INTERRUPT);
        idt[3]   = CreateIDTEntry((uint32_t) IRQException3,   0x8, ENABLED_R0_INTERRUPT);
        idt[4]   = CreateIDTEntry((uint32_t) IRQException4,   0x8, ENABLED_R0_INTERRUPT);
        idt[5]   = CreateIDTEntry((uint32_t) IRQException5,   0x8, ENABLED_R0_INTERRUPT);
        idt[6]   = CreateIDTEntry((uint32_t) IRQException6,   0x8, ENABLED_R0_INTERRUPT);
        idt[7]   = CreateIDTEntry((uint32_t) IRQException7,   0x8, ENABLED_R0_INTERRUPT);
        idt[8]   = CreateIDTEntry((uint32_t) IRQException8,   0x8, ENABLED_R0_INTERRUPT);
        idt[9]   = CreateIDTEntry((uint32_t) IRQException9,   0x8, ENABLED_R0_INTERRUPT);
        idt[10]  = CreateIDTEntry((uint32_t) IRQException10,  0x8, ENABLED_R0_INTERRUPT);
        idt[11]  = CreateIDTEntry((uint32_t) IRQException11,  0x8, ENABLED_R0_INTERRUPT);
        idt[12]  = CreateIDTEntry((uint32_t) IRQException12,  0x8, ENABLED_R0_INTERRUPT);
        idt[13]  = CreateIDTEntry((uint32_t) IRQException13,  0x8, ENABLED_R0_INTERRUPT);
        idt[14]  = CreateIDTEntry((uint32_t) IRQException14,  0x8, ENABLED_R0_INTERRUPT);
        idt[15]  = CreateIDTEntry((uint32_t) IRQException15,  0x8, ENABLED_R0_INTERRUPT);
        idt[16]  = CreateIDTEntry((uint32_t) IRQException16,  0x8, ENABLED_R0_INTERRUPT);
        idt[17]  = CreateIDTEntry((uint32_t) IRQException17,  0x8, ENABLED_R0_INTERRUPT);
        idt[18]  = CreateIDTEntry((uint32_t) IRQException18,  0x8, ENABLED_R0_INTERRUPT);
        idt[19]  = CreateIDTEntry((uint32_t) IRQException19,  0x8, ENABLED_R0_INTERRUPT);
        idt[20]  = CreateIDTEntry((uint32_t) IRQException20,  0x8, ENABLED_R0_INTERRUPT);
        idt[21]  = CreateIDTEntry((uint32_t) IRQException21,  0x8, ENABLED_R0_INTERRUPT);
        idt[22]  = CreateIDTEntry((uint32_t) IRQException22,  0x8, ENABLED_R0_INTERRUPT);
        idt[23]  = CreateIDTEntry((uint32_t) IRQException23,  0x8, ENABLED_R0_INTERRUPT);
        idt[24]  = CreateIDTEntry((uint32_t) IRQException24,  0x8, ENABLED_R0_INTERRUPT);
        idt[25]  = CreateIDTEntry((uint32_t) IRQException25,  0x8, ENABLED_R0_INTERRUPT);
        idt[26]  = CreateIDTEntry((uint32_t) IRQException26,  0x8, ENABLED_R0_INTERRUPT);
        idt[27]  = CreateIDTEntry((uint32_t) IRQException27,  0x8, ENABLED_R0_INTERRUPT);
        idt[28]  = CreateIDTEntry((uint32_t) IRQException28,  0x8, ENABLED_R0_INTERRUPT);
        idt[29]  = CreateIDTEntry((uint32_t) IRQException29,  0x8, ENABLED_R0_INTERRUPT);
        idt[30]  = CreateIDTEntry((uint32_t) IRQException30,  0x8, ENABLED_R0_INTERRUPT);

        // PIT
        idt[offset+0]  = CreateIDTEntry((uint32_t) Multitask::IRQ0,  0x8, ENABLED_R0_INTERRUPT);

        // 0x80 interrupts
        idt[0x80] = CreateIDTEntry((uint32_t)Multitask::IRQ80, 0x8, ENABLED_R3_INTERRUPT);
    }
    
    void OnInterrupt(const uint32_t irq, const StackFrameRegisters registers)
    {
        switch (irq)
        {
            case 0x0: // IRQ
                assert(false); // Shouldn't be using the generic interrupt handler!
            break;

            default:
                UART::WriteString("[IRQ] Unrecognised IRQ ");
                UART::WriteNumber(irq);
                UART::WriteString(" ----- esp ");
                UART::WriteNumber(registers.esp);
                UART::WriteString("\n");
            break;
        }

        PIC::EndInterrupt((uint8_t)irq);
    }

    void OnException(const uint32_t irq, const StackFrameRegisters registers)
    {
        static char const* exceptions[32] =
        {
            "Divide by zero",               "Debug",
            "Non-maskable interrupt",       "Breakpoint",
            "Overflow",                     "Bound range exceeded",
            "Invalid opcode",               "Device not available",
            "Double fault",                 "Coprocessor segment overrun",
            "Invalid task state segment",   "Segment not present",
            "Stack-segment fault",          "General protection fault",
            "Page fault",                   "Reserved",
            "x87 floating-point exception", "Alignment check",
            "Machine check",                "SIMD floating-point exception",
            "Virtualisation exception",     "Reserved",
            "Reserved",                     "Reserved",
            "Reserved",                     "Reserved",
            "Reserved",                     "Reserved",
            "Reserved",                     "Reserved",
            "Security exception",
        };

        UART::WriteString("\n-----------------------\n");

        UART::WriteString("[Error] ");
        UART::WriteString(exceptions[irq]);
        UART::WriteString(" exception occured - IRQ ");
        UART::WriteNumber(irq);
        UART::WriteString("\n");

        const auto PrintRegister = [](const char* name, const uint32_t value)
        {
            UART::WriteString(name);
            UART::WriteString(": ");
            UART::WriteNumber(value);
            UART::WriteString("\n");
        };

        PrintRegister("eax", registers.eax);
        PrintRegister("ecx", registers.ecx);
        PrintRegister("edx", registers.edx);
        PrintRegister("ebx", registers.ebx);
        PrintRegister("esp", registers.esp);
        PrintRegister("ebp", registers.ebp);
        PrintRegister("esi", registers.esi);

        UART::WriteString("-----------------------\n");

        // Blue screen
        using namespace Framebuffer;
        for (uint32_t y = 0; y < sFramebuffer.height; ++y)
            for (uint32_t x = 0; x < sFramebuffer.width; ++x)
                sFramebuffer.SetPixel(x, y, GetColour(0, 0, 255));

        // TODO: halt task if running in user-mode
        while (true) asm("hlt");

        PIC::EndInterrupt((uint8_t) irq);
    }
}