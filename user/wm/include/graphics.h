#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "stdlib.h"
#include "font.h"
#include "colours.h"
#include "window.h"

#define BAR_HEIGHT 20
#define BAR_PADDING 4

class Graphics
{
public:
    Graphics();
    void Init(uint32_t width, uint32_t height, uint32_t address, uint32_t pitch);
    ~Graphics();

    inline uint32_t GetColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
    {
        return a << 24 | r << 16 | g << 8 | b;
    }

    void DrawRect(uint32_t x, uint32_t y, uint32_t rectWidth, uint32_t rectHeight, uint32_t colour, Window& window);
    void DrawWindow(Window& window);
    void OnWindowTitleChange(Window& window);
    void OnWindowMove(Window& window);
    void OnWindowDestroy(Window& window);

    void DrawChar(char c, uint32_t x, uint32_t y, uint32_t colour, Window& window, uint32_t backgroundColour = WINDOW_BACKGROUND_COLOUR);
    void DrawString(char const* string, uint32_t x, uint32_t y, uint32_t colour, Window& window, uint32_t backgroundColour = WINDOW_BACKGROUND_COLOUR);
    void DrawNumber(uint32_t number, uint32_t x, uint32_t y, uint32_t colour, bool hex, Window& window);

    void Blit(void* data);
    void DrawFrame(Window* pWindows);

    inline uint32_t GetDigits(uint32_t number, uint32_t base)
    {
        size_t i = number;
        size_t nDigits = 1;
        while (i/=base) nDigits++;
        return nDigits;
    };

    uint32_t m_Width;
    uint32_t m_Height;

private:
    uint32_t m_Address;
    uint32_t m_Pitch;

    void* m_Background;
    void* m_Buffer;

    struct Rect
    {
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
        Window* window;
    };

    Rect m_DirtyRects[64];
    uint32_t nRects = 0;

    void PushRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Window* window)
    {
        if (nRects == sizeof(m_DirtyRects) / sizeof(m_DirtyRects[0])) return;

        m_DirtyRects[nRects].x = x; m_DirtyRects[nRects].width = width;
        m_DirtyRects[nRects].y = y; m_DirtyRects[nRects].height = height;
        m_DirtyRects[nRects].window = window;
        nRects++;
    }

    void ConfineWindowToReasonableBounds(Window& window);
};



#endif