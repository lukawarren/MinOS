#include "uart.h"
#include "io.h"
#include "vga.h"
#include "../stdlib.h"

UART::UART(COM com)
{
    m_Com = com;
    outb((uint16_t)m_Com + 1, 0x00);    // Disable all interrupts
    outb((uint16_t)m_Com + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb((uint16_t)m_Com + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb((uint16_t)m_Com + 1, 0x00);    //                  (hi byte)
    outb((uint16_t)m_Com + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb((uint16_t)m_Com + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb((uint16_t)m_Com + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void UART::WriteChar(char c)
{
    auto IsTransitEmpty = [&]() { return inb((uint16_t)m_Com + 5) & 0x20; };

    while (!IsTransitEmpty()) {}
    outb((uint16_t)m_Com + 0, c);
}

void UART::WriteString(char const* string, bool newLine)
{
    for (size_t i = 0; i < strlen(string); ++i) 
    {
        WriteChar(string[i]);
    }
    if (newLine) WriteChar('\n');
}

UART::~UART()
{
    
}