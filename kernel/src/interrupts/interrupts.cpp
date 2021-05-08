#include "interrupts/interrupts.h"
#include "stdout/uart.h"
#include "cpu/pic.h"
#include "cpu/cpu.h"

namespace Interrupts
{
    void OnInterrupt(const uint32_t irq, const StackFrameRegisters registers)
    {
        UART::WriteString("Interupt ");
        UART::WriteNumber(irq);
        UART::WriteString(" received - esp ");

        if (irq == 1) CPU::inb(0x60);
        UART::WriteNumber(registers.esp);
        UART::WriteString("\n");

        PIC::EndInterrupt((uint8_t)irq);
    }
}