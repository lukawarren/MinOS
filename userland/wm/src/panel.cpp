#include "panel.h"
#include "graphics.h"

// Colours
constexpr uint32_t cPanelBackground    = Graphics::GetColour(212, 208, 200);
constexpr uint32_t cPanelLeadingEdge   = Graphics::GetColour(128, 128, 128);
constexpr uint32_t cPanelTrim          = Graphics::GetColour(255, 255, 255);

uint32_t Graphics::Panel::GetPixel(const uint32_t screenX, const uint32_t screenY) const
{
    // Convert to local space
    const uint32_t x = screenX - m_X;
    const uint32_t y = screenY - m_Y;

    // Right leading edge
    if (y > 0 && x == m_Width-1) return cPanelLeadingEdge;

    // Bottom leading edge
    if (x > 0 && y == m_Height-1) return cPanelLeadingEdge;

    // White trim
    if (x > 0 && x < m_Width && y == 1) return cPanelTrim;
    if (y > 0 && y < m_Height && x == 1) return cPanelTrim;

    return cPanelBackground;
}
