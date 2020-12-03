#include "vga.h"
#include "../stdlib.h"
#include "../io/io.h"
#include "charset.h"

Framebuffer VGA_framebuffer;

uint32_t VGA_charRows;
uint32_t VGA_charColumns;

uint32_t VGA_row = 0;
uint32_t VGA_column = 0;

void VGA_Init(Framebuffer fb)
{
    VGA_framebuffer = fb;
    VGA_charRows = fb.height / (CHAR_HEIGHT*CHAR_SCALE);
    VGA_charColumns = fb.width / CHAR_WIDTH;
}

void VGA_PutPixel(size_t x, size_t y, uint32_t colour)
{
    size_t index = x*4 + y*VGA_framebuffer.pitch;
    *(uint32_t*)((uint32_t)VGA_framebuffer.address + index) = colour;
}

void VGA_WriteChar(char c, size_t x, size_t y, uint32_t colour)
{
    const uint8_t* bitmap = GetFontFromChar(c);

    for (int w = 0; w < CHAR_WIDTH; ++w)
    {
        for (int h = 0; h < CHAR_HEIGHT*CHAR_SCALE; ++h)
        {
            uint8_t mask = 1 << (w);

            size_t xPos = x * CHAR_WIDTH + w;
            size_t yPos = y * CHAR_HEIGHT*CHAR_SCALE + h;
            size_t index = xPos*4 + yPos*VGA_framebuffer.pitch;
            if (bitmap[h/CHAR_SCALE] & mask) *(uint32_t*)((uint32_t)VGA_framebuffer.address + index) = colour;
            else *(uint32_t*)((uint32_t)VGA_framebuffer.address + index) = VGA_COLOUR_BLACK;       
        }
    }
}

void VGA_WriteString(char const* string, bool newLine, uint32_t colour)
{
    for (size_t i = 0; i < strlen(string); ++i) 
    {
        VGA_WriteChar(string[i], VGA_column, VGA_row, colour);
        if (++VGA_column >= VGA_charColumns)
        {
            VGA_column = 0;
            if (++VGA_row >= VGA_charRows-1) VGA_row = 0;
        }
    }
    if (newLine)
    {
        VGA_column = 0;
        if (++VGA_row >= VGA_charRows-1) { VGA_row = 0; VGA_Clear(); }
    }
}

void VGA_Clear(uint32_t colour)
{
    for (size_t i = 0; i < VGA_framebuffer.width; ++i)
    {
        for (size_t j = 0; j < VGA_framebuffer.height; ++j) VGA_PutPixel(i, j, colour);
    }
}

template <>
void VGA_printf<uint64_t, true>(uint64_t data, bool newLine, uint32_t colour)
{
    auto hexToASCII = [](const uint64_t number) 
    {
        char value = number % 16 + 48;
        if (value > 57) value += 7;
        return value;
    };
    auto getNthDigit = [](const uint64_t number, const size_t digit) { return (number >> (digit*4)) & 0xF; };

    VGA_WriteString("0x", false, colour); 
    for (size_t d = 0; d < 16; ++d) 
    { 
        VGA_WriteChar(hexToASCII(getNthDigit(data, 16 - d - 1)), VGA_column, VGA_row, colour); 
        if (++VGA_column >= VGA_charColumns)
		{
			VGA_column = 0;
			if (++VGA_row >= VGA_charRows-1) VGA_row = 0;
		}
    }

    if (newLine)
    { 
        VGA_column = 0; 
        if (++VGA_row >= VGA_charRows-1) { VGA_row = 0; VGA_Clear(); } 
    }
}