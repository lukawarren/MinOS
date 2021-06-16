#include "bar.h"

// Colours
constexpr uint32_t cWindowBarShade     = Graphics::GetColour(150, 150, 150);
constexpr uint32_t cWindowLineShade    = Graphics::GetColour(100, 100, 100);

uint32_t Graphics::Bar::GetPixel(const uint32_t x, const uint32_t y) const
{
    const auto nLinePadding = 5;
    const auto nTextMargin = 80;
    const auto nButtonsMargin = 30;
    
    const bool bIsLine = y >= nLinePadding && y <= m_Height - nLinePadding &&
                        x <= m_Width - nLinePadding && x >= nLinePadding &&
                        x >= nTextMargin && x <= m_Width - nButtonsMargin && y % 2 == 0;

    return bIsLine ? cWindowLineShade : cWindowBarShade;
}

