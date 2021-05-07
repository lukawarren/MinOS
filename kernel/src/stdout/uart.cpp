#include "stdout/uart.h"
#include "cpu.h"

namespace UART
{
    static COM sCom;
}

void UART::InitUART(const COM com)
{
    sCom = com;
    CPU::outb((uint16_t)sCom + 1, 0x00); // Disable all interrupts
    CPU::outb((uint16_t)sCom + 3, 0x80); // Enable DLAB (set baud rate divisor)
    CPU::outb((uint16_t)sCom + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    CPU::outb((uint16_t)sCom + 1, 0x00); //                  (hi byte)
    CPU::outb((uint16_t)sCom + 3, 0x03); // 8 bits, no parity, one stop bit
    CPU::outb((uint16_t)sCom + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    CPU::outb((uint16_t)sCom + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

void UART::WriteChar(const char c)
{
    // Wait for transit to be empty first
    auto IsTransitEmpty = [&]() { return CPU::inb((uint16_t)sCom + 5) & 0x20; };
    while (!IsTransitEmpty()) {}
    CPU::outb((uint16_t)sCom + 0, c);
}
