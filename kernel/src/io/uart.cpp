#include "io/uart.h"
#include "cpu/cpu.h"
#include "klib.h"

namespace uart
{
    enum class COM
    {
        COM1 = 0x3F8,
        COM2 = 0x2F8,
        COM3 = 0x3E8,
        COM4 = 0x2E8
    };

    constexpr COM com = COM::COM1;

    void init()
    {
        cpu::outb((uint16_t)com + 1, 0x00); // Disable all interrupts
        cpu::outb((uint16_t)com + 3, 0x80); // Enable DLAB (set baud rate divisor)
        cpu::outb((uint16_t)com + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
        cpu::outb((uint16_t)com + 1, 0x00); //                  (hi byte)
        cpu::outb((uint16_t)com + 3, 0x03); // 8 bits, no parity, one stop bit
        cpu::outb((uint16_t)com + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
        cpu::outb((uint16_t)com + 4, 0x0B); // IRQs enabled, RTS/DSR set
    }

    void write_char(const char c)
    {
        // Wait for transit to be empty first
        auto is_transit_empty = [&]() { return cpu::inb((uint16_t)com + 5) & 0x20; };
        while (!is_transit_empty()) {}
        cpu::outb((uint16_t)com, (uint8_t)c);
    }

    void write_string(const char* string)
    {
        for (size_t i = 0; i < strlen(string); ++i)
            write_char(string[i]);
    }

    void write_number(const uint32_t number)
    {
        // Get number of digits
        int nDigits = 1;
        uint32_t i = number;
        while (i /= 10) ++nDigits;

        // Print digits "in reverse"
        const char digits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        for (int digit = nDigits-1; digit >= 0; --digit)
        {
            const int nthDigit = int(number / pow(10, (size_t)digit) % 10);
            write_char(digits[nthDigit]);
        }
    }

}