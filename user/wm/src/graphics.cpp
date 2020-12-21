#include "graphics.h"

Graphics::Graphics() {}

void Graphics::Init(uint32_t width, uint32_t height, uint32_t address, uint32_t pitch)
{
    m_Width = width; m_Height = height;
    m_Address = address; m_Pitch = pitch;
}

void Graphics::DrawRect(uint32_t x, uint32_t y, uint32_t rectWidth, uint32_t rectHeight, uint32_t colour)
{
    // TODO: Optimise!

    for (uint32_t i = x; i < x + rectWidth; ++i)
        for (uint32_t j = y; j < y + rectHeight; ++j)
            PutPixel(i, j, colour);
}

void Graphics::DrawBackground()
{
    DrawRect(0, 0, m_Width, m_Height, 0);
}

void Graphics::DrawWindow(const char* sTitle, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    const uint32_t outlineWidth = 3;

    // Draw outlilne
    DrawRect(x - outlineWidth, y - outlineWidth, width + outlineWidth*2, height + outlineWidth*2, GetColour(68, 68, 68));

    // Draw window
    DrawRect(x, y, width, height, GetColour(51, 51, 51));
}

Graphics::~Graphics() {}