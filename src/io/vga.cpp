#include "vga.h"
#include "../stdlib.h"
#include "io.h"

VGA_Entry* VGA_BUFFER = (VGA_Entry*)0xB8000;
size_t VGA_ROW = 0;
size_t VGA_COLUMN = 0;

void VGA_WriteChar(char c, size_t x, size_t y, uint8_t colour)
{
    const size_t index = y * VGA_WIDTH + x;
    VGA_BUFFER[index] = {static_cast<uint8_t>(c), colour | VGA_COLOUR_BLUE << 4};
}

void VGA_WriteString(char const* string, bool newLine, uint8_t colour)
{
    for (size_t i = 0; i < strlen(string); ++i) 
    {
        VGA_WriteChar(string[i], VGA_ROW, VGA_COLUMN, colour);
        if (++VGA_ROW > VGA_WIDTH)
        {
            VGA_ROW = 0;
            if (++VGA_COLUMN > VGA_HEIGHT) VGA_COLUMN = 0;
        }
    }
    if (newLine)
    {
        VGA_ROW = 0;
        VGA_COLUMN++;
    }
    VGA_MoveCursor(VGA_ROW, VGA_COLUMN);
}

void VGA_Clear(uint8_t colour)
{
    for (size_t i = 0; i < VGA_WIDTH; ++i)
    {
        for (size_t j = 0; j < VGA_HEIGHT; ++j) VGA_WriteChar(' ', i, j, colour);
    }
}

void VGA_EnableCursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | 0);
 
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | 0);
}

void VGA_DisableCursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void VGA_MoveCursor(size_t x, size_t y)
{
    size_t pos = (y + 1) * VGA_WIDTH + (x);
    
    //if (x >= VGA_WIDTH) pos = (y+2) * VGA_WIDTH;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

template <>
void VGA_printf<uint64_t, true>(uint64_t data, bool newLine, uint8_t colour)
{
    auto hexToASCII = [](const uint64_t number) 
    {
        char value = number % 16 + 48;
        if (value > 57) value += 7;
        return value;
    };
    auto getNthDigit = [](const uint64_t number, const size_t digit) { return (number >> (digit*4)) & 0xF; };

    VGA_WriteString("0x", false, colour); 
    for (size_t d = 0; d < 16; ++d) { VGA_WriteChar(hexToASCII(getNthDigit(data, 16 - d - 1)), VGA_ROW, VGA_COLUMN, colour); VGA_ROW++; }

    if (newLine) { VGA_ROW = 0; VGA_COLUMN++; }

    VGA_MoveCursor(VGA_ROW, VGA_COLUMN);
}