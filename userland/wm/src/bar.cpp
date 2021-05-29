#include "bar.h"
#include "graphics.h"

// Colours
constexpr uint32_t cWindowBarShade     = Graphics::GetColour(150, 150, 150);
constexpr uint32_t cWindowLineShade    = Graphics::GetColour(100, 100, 100);

uint32_t Graphics::Bar::GetPixel(const uint32_t screenX, const uint32_t screenY) const
{
    using namespace Graphics;

    const auto nLinePadding = 5;

    // Convert to local space
    const uint32_t x = screenX - m_X;
    const uint32_t y = screenY - m_Y;

    const bool bIsLine = y >= nLinePadding && y <= m_Height - nLinePadding &&
                        x <= m_Width - nLinePadding && x >= nLinePadding && y % 2 == 0;

    return bIsLine ? cWindowLineShade : cWindowBarShade;
}
