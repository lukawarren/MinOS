#include "panel.h"
#include "graphics.h"

uint32_t Graphics::Panel::GetPixel(const uint32_t x, const uint32_t y) const
{
    // Right leading edge
    if (y > 0 && x == m_Width-1) return cPanelLeadingEdge;

    // Bottom leading edge
    if (x > 0 && y == m_Height-1) return cPanelLeadingEdge;

    // White trim
    if (m_cPanelColour == cPanelBackground && x < m_Width  && y == 0) return cPanelTrim;
    if (m_cPanelColour == cPanelBackground && y < m_Height && x == 0) return cPanelTrim;

    return m_cPanelColour;
}
