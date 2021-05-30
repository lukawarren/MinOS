#include "panel.h"
#include "graphics.h"

Graphics::Panel::Panel(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const uint32_t colour) :
    Widget(width, height, x, y)
{
    m_cPanelColour = colour;
}        

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
    if (m_cPanelColour == cPanelBackground && x < m_Width  && y == 0) return cPanelTrim;
    if (m_cPanelColour == cPanelBackground && y < m_Height && x == 0) return cPanelTrim;

    return m_cPanelColour;
}
