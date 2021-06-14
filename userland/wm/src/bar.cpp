#include "bar.h"
#include "graphics.h"

// Colours
constexpr uint32_t cWindowBarShade     = Graphics::GetColour(150, 150, 150);
constexpr uint32_t cWindowLineShade    = Graphics::GetColour(100, 100, 100);

Graphics::Bar::Bar(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y) : Widget(width, height, x, y)
{
    for (uint32_t _x = 0; _x < width; ++_x)
        for (uint32_t _y = 0; _y < height; ++_y)
            m_pBitmap[_y * width + _x] = GetPixel(x + _x, y + _y);
}

uint32_t Graphics::Bar::GetPixel(const uint32_t screenX, const uint32_t screenY) const
{
    const auto nLinePadding = 5;
    const auto nTextMargin = 80;
    const auto nButtonsMargin = 30;

    // Convert to local space
    const uint32_t x = screenX - m_X;
    const uint32_t y = screenY - m_Y;

    const bool bIsLine = y >= nLinePadding && y <= m_Height - nLinePadding &&
                        x <= m_Width - nLinePadding && x >= nLinePadding &&
                        x >= nTextMargin && x <= m_Width - nButtonsMargin && y % 2 == 0;

    return bIsLine ? cWindowLineShade : cWindowBarShade;
}

void Graphics::Bar::Redraw(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y)
{
    // Resize and fill buffer if width changed
    if (width != m_Width)
    {
        free(m_pBitmap);
        m_pBitmap = (uint32_t*) malloc(sizeof(uint32_t) * width * height);
        
        for (uint32_t _x = 0; _x < width; ++_x)
            for (uint32_t _y = 0; _y < height; ++_y)
                m_pBitmap[_y * width + _x] = GetPixel(x + _x, y + _y);
    }
    
    m_Width = width;
    m_Height = height;
    m_X = x;
    m_Y = y;
}
