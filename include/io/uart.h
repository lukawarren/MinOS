#pragma once
#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stddef.h>
#include "../stdlib.h"

class UART
{
public:

    enum COM
    {
        COM1 = 0x3F8,
        COM2 = 0x2F8,
        COM3 = 0x3E8,
        COM4 = 0x2E8
    };

    UART(COM com);
    ~UART();

    void WriteChar(char c);
    void WriteString(char const* string, bool newLine = true);

    template <typename T>
    void printf(T* data, bool newLine = true)
    {
        WriteString(static_cast<char const*>(data), newLine);
    }

    template <typename T, bool hex = false>
    void printf(T data, bool newLine = true)
    {
        // Get number of digits
        size_t i = data;
        size_t nDigits = 1;
        while (i/=(hex ? 16 : 10)) nDigits++;

        auto digitToASCII = [](const size_t number) { return (char)('0' + number); };
        auto hexToASCII = [](const size_t number) 
        {
            char value = number % 16 + 48;
            if (value > 57) value += 7;
            return value;
        };
        auto getNthDigit = [](const size_t number, const size_t digit, const size_t base) { return int((number / pow(base, digit)) % base); };

        if (hex) 
        { 
            WriteString("0x", false); 
            for (size_t d = 0; d < nDigits; ++d) WriteChar(hexToASCII(getNthDigit(data, nDigits - d - 1, 16)));
        }
        else
        {
            for (size_t d = 0; d < nDigits; ++d) WriteChar(digitToASCII(getNthDigit(data, nDigits - d - 1, 10)));
        }

        if (newLine) WriteChar('\n');
    }

    COM m_Com;

    static UART* pCOM;
};

#endif