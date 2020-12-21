#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "stdlib.h"
#include "font.h"

class Graphics
{
public:
    Graphics();
    void Init(uint32_t width, uint32_t height, uint32_t address, uint32_t pitch);
    ~Graphics();

    inline void PutPixel(size_t x, size_t y, uint32_t colour)
    {
        size_t index = x*4 + y*m_Pitch;
        *(uint32_t*)((uint32_t)m_Address + index) = colour;
    }

    inline uint32_t GetColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
    {
        return a << 24 | r << 16 | g << 8 | b;
    }

    void DrawRect(uint32_t x, uint32_t y, uint32_t rectWidth, uint32_t rectHeight, uint32_t colour);
    void DrawBackground();
    void DrawWindow(const char* sTitle, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    void DrawChar(char c, uint32_t x, uint32_t y, uint32_t colour);
    void DrawString(char const* string, uint32_t x, uint32_t y, uint32_t colour);

    void Blit(void* data);

    template <typename T, bool hex = false, size_t digits = 0>
    void DrawNumber(T data, uint32_t x, uint32_t y, uint32_t colour)
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
            DrawString("0x", x, y, colour);
            for (size_t d = 0; d < nDigits; ++d)
            { 
                DrawChar(hexToASCII(getNthDigit(data, nDigits - d - 1, 16)), x+CHAR_WIDTH*(d+2), y, colour);
            }
        }
        else
        {
            for (size_t d = 0; d < nDigits; ++d)
            { 
                DrawChar(digitToASCII(getNthDigit(data, nDigits - d - 1, 10)), x+CHAR_WIDTH*d, y, colour); 
            }
        }
    }

private:
    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_Address;
    uint32_t m_Pitch;
};



#endif