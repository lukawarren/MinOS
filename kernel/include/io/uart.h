#ifndef UART_H
#define UART_H

#include <stdint.h>

namespace UART
{
    enum COM
    {
        COM1 = 0x3F8,
        COM2 = 0x2F8,
        COM3 = 0x3E8,
        COM4 = 0x2E8
    };

    void Init(const COM com = COM1);
    void WriteChar(const char c);
    void WriteString(char const* string);
    void WriteNumber(const uint32_t number);
}

#endif