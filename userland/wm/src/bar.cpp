#include "bar.h"

// Colours (N out of 3)
constexpr uint32_t cWindowBarShade     = 2;
constexpr uint32_t cWindowLineShade    = 3;

uint32_t Graphics::Bar::GetPixel(const uint32_t x, const uint32_t y) const
{
    const auto nLinePadding = 5;
    const auto nTextMargin = 80;
    const auto nButtonsMargin = 30;
    
    const bool bIsLine = y >= nLinePadding && y <= m_Height - nLinePadding &&
                        x <= m_Width - nLinePadding && x >= nLinePadding &&
                        x >= nTextMargin && x <= m_Width - nButtonsMargin && y % 2 == 0;

    // Work out gradient with *integer maths* only :-o
    const uint32_t progress = (x * 255) / (m_Width);

    // 255 * 3 = 765
    return bIsLine ? 
        GetColour(progress * cWindowLineShade * r / 765, progress * cWindowLineShade * g / 765, progress * cWindowLineShade * b / 765) :
        GetColour(progress * cWindowBarShade * r / 765, progress * cWindowBarShade * g / 765, progress * cWindowBarShade * b / 765);
}

void Graphics::Bar::SetColour(const uint32_t colour)
{
    r = uint8_t(colour >> 16);
    g = uint8_t(colour >> 8);
    b = uint8_t(colour);
}
