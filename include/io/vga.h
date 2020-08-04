#pragma once
#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>
#include "../stdlib.h"

enum VGA_Colour
{
    VGA_COLOUR_BLACK = 0,
    VGA_COLOUR_BLUE = 1,
    VGA_COLOUR_GREEN = 2,
    VGA_COLOUR_CYAN = 3,
    VGA_COLOUR_RED = 4,
    VGA_COLOUR_MAGENTA = 5,
    VGA_COLOUR_BROWN = 6,
    VGA_COLOUR_LIGHT_GREY = 7,
    VGA_COLOUR_DARK_GREY = 8,
    VGA_COLOUR_LIGHT_BLUE = 9,
    VGA_COLOUR_LIGHT_GREEN = 10,
    VGA_COLOUR_LIGHT_CYAN = 11,
    VGA_COLOUR_LIGHT_RED = 12,
    VGA_COLOUR_LIGHT_MAGENTA = 13,
    VGA_COLOUR_LIGHT_BROWN = 14,
    VGA_COLOUR_WHITE = 15,
};

struct VGA_Entry
{
    uint8_t character;
    uint8_t colour;
};

constexpr size_t VGA_WIDTH = 80;
constexpr size_t VGA_HEIGHT = 25;

extern VGA_Entry* VGA_BUFFER;

extern size_t VGA_ROW;
extern size_t VGA_COLUMN;

void VGA_EnableCursor();
void VGA_DisableCursor();
void VGA_MoveCursor(size_t x, size_t y);

void VGA_WriteChar(char c, size_t x, size_t y, uint8_t colour = VGA_COLOUR_WHITE);
void VGA_WriteString(char const* string, bool newLine = true, uint8_t colour = VGA_COLOUR_WHITE);

template <typename T>
void VGA_printf(T* data, bool newLine = true, uint8_t colour = VGA_COLOUR_WHITE)
{
    VGA_WriteString(static_cast<char const*>(data), newLine, colour);
}

template <typename T, bool hex = false>
void VGA_printf(T data, bool newLine = true, uint8_t colour = VGA_COLOUR_WHITE)
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
        VGA_WriteString("0x", false, colour); 
        for (size_t d = 0; d < nDigits; ++d) { VGA_WriteChar(hexToASCII(getNthDigit(data, nDigits - d - 1, 16)), VGA_ROW, VGA_COLUMN, colour); VGA_ROW++; }
    }
    else
    {
        for (size_t d = 0; d < nDigits; ++d) { VGA_WriteChar(digitToASCII(getNthDigit(data, nDigits - d - 1, 10)), VGA_ROW, VGA_COLUMN, colour); VGA_ROW++; }
    }

    if (newLine) { VGA_ROW = 0; VGA_COLUMN++; }

    VGA_MoveCursor(VGA_ROW, VGA_COLUMN);
}

void VGA_Clear(uint8_t colour = VGA_COLOUR_WHITE | VGA_COLOUR_BLACK << 4);

#endif