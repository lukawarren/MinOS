#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "stdlib.h"

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

private:
    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_Address;
    uint32_t m_Pitch;
};



#endif