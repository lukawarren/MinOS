#include "stdout/uart.h"
#include "cpu/cpu.h"
#include "kstdlib.h"

namespace UART
{
    static COM sCom;

    void Init(const COM com)
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

    void WriteChar(const char c)
    {
        // Wait for transit to be empty first
        auto IsTransitEmpty = [&]() { return CPU::inb((uint16_t)sCom + 5) & 0x20; };
        while (!IsTransitEmpty()) {}
        CPU::outb((uint16_t)sCom, c);
    }

    void WriteString(char const* string)
    {
        for (size_t i = 0; i < strlen(string); ++i) WriteChar(string[i]);
    }

    void WriteNumber(const uint32_t number)
    {
        // Get number of digits
        size_t nDigits = 1;
        size_t i = number;
        while (i /= 10) nDigits++;

        // Print digits "in reverse"
        const char digits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        for (int digit = nDigits-1; digit >= 0; --digit)
        {
            const size_t nthDigit = int(number / pow(10, digit) % 10);
            WriteChar(digits[nthDigit]);
        }
    }

}
