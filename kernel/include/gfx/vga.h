#pragma once
#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>
#include "stdlib.h"

struct Framebuffer
{
    unsigned int width;
    unsigned int height;
    uint32_t pitch;
    uint32_t* address;
};
extern Framebuffer VGA_framebuffer;

extern uint32_t VGA_charRows;
extern uint32_t VGA_charColumns;
extern uint32_t VGA_row;
extern uint32_t VGA_column;

enum VGA_Colour
{
    VGA_COLOUR_BLACK = 0,
    VGA_COLOUR_WHITE = 0xFFFFFFFF,
    VGA_COLOUR_LIGHT_GREEN = 0x55FF55,
    VGA_COLOUR_LIGHT_RED = 0xFF5555,
    VGA_COLOUR_GREEN = 0x00A300,
    VGA_COLOUR_LIGHT_YELLOW = 0xFFFF55
};

void VGA_Init(Framebuffer fb);

void VGA_WriteChar(char c, size_t x, size_t y, uint32_t colour = VGA_COLOUR_WHITE);
void VGA_WriteString(char const* string, bool newLine = true, uint32_t colour = VGA_COLOUR_WHITE);
void VGA_Clear(uint32_t colour = VGA_COLOUR_BLACK);

void VGA_PutPixel(size_t x, size_t y, uint32_t colour);

template <typename T>
void VGA_printf(T* data, bool newLine = true, uint32_t colour = VGA_COLOUR_WHITE)
{
    VGA_WriteString(static_cast<char const*>(data), newLine, colour);
}

template <typename T, bool hex = false, size_t digits = 0>
void VGA_printf(T data, bool newLine = true, uint32_t colour = VGA_COLOUR_WHITE)
{
    // Get number of digits
    size_t nDigits = digits;
    if (nDigits == 0)
    {
        size_t i = data;
        nDigits = 1;
        while (i/=(hex ? 16 : 10)) nDigits++;
    }

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
        VGA_WriteString("0x", false, colour); 
        for (size_t d = 0; d < nDigits; ++d)
		{ 
			VGA_WriteChar(hexToASCII(getNthDigit(data, nDigits - d - 1, 16)), VGA_column, VGA_row, colour);
			if (++VGA_column >= VGA_charColumns)
			{
				VGA_column = 0;
				if (++VGA_row >= VGA_charRows-1) VGA_row = 0;
			}
		}
    }
    else
    {
        for (size_t d = 0; d < nDigits; ++d)
		{ 
			VGA_WriteChar(digitToASCII(getNthDigit(data, nDigits - d - 1, 10)), VGA_column, VGA_row, colour); 
			if (++VGA_column > VGA_charColumns)
			{
				VGA_column = 0;
				if (++VGA_row >= VGA_charRows-1) VGA_row = 0;
			}
		}
    }

    if (newLine)
    { 
        VGA_column = 0; 
        if (++VGA_row >= VGA_charRows-1) { VGA_row = 0; VGA_Clear(); } 
    }
}

template <>
void VGA_printf<uint64_t, true>(uint64_t data, bool newLine, uint32_t colour);



#endif