#include "interrupts/interrupts.h"
#include "stdout/uart.h"
#include "cpu/pic.h"

namespace Interrupts
{
    void OnInterrupt(const uint32_t irq, const StackFrameRegisters registers)
    {
        UART::WriteString("Interupt ");
        UART::WriteNumber(irq);
        UART::WriteString(" received\n");

        PIC::EndInterrupt((uint8_t)irq);
    }
}