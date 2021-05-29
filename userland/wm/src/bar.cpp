#include "bar.h"
#include "graphics.h"

// Colours
constexpr uint32_t cWindowBarShade     = Graphics::GetColour(150, 150, 150);
constexpr uint32_t cWindowLineShade    = Graphics::GetColour(100, 100, 100);

void Graphics::Bar::Draw(const uint32_t screenX, const uint32_t screenY) const
{
    using namespace Graphics;

    const auto nLinePadding = 5;

    for (uint32_t x = 0; x < m_Width; ++x)
    {
        for (uint32_t y = 0; y < m_Height; ++y)
        {
            // Add lines
            const bool bIsLine = y >= nLinePadding && y <= m_Height - nLinePadding &&
                                x <= m_Width - nLinePadding && x >= nLinePadding && y % 2 == 0;
            WritePixel(m_X + x, m_Y + y, bIsLine ? cWindowLineShade : cWindowBarShade);
      }
    }
}
