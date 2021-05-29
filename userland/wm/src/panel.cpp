#include "panel.h"
#include "graphics.h"

// Colours
constexpr uint32_t cPanelBackground    = Graphics::GetColour(212, 208, 200);
constexpr uint32_t cPanelLeadingEdge   = Graphics::GetColour(128, 128, 128);
constexpr uint32_t cPanelTrim          = Graphics::GetColour(255, 255, 255);

void Graphics::Panel::Draw(const uint32_t screenX, const uint32_t screenY) const
{
    using namespace Graphics;

    // Draw background (save for leading edge)
    for (uint32_t y = 0; y < m_Height; ++y)
        for (uint32_t x = 0; x < m_Width; ++x)
            WritePixel(m_X + x, m_Y + y, cPanelBackground);

    // Draw right leading edge
    for (uint32_t y = 1; y < m_Height; ++y)
        WritePixel(m_X + m_Width - 1, m_Y + y, cPanelLeadingEdge);

    // Draw bottom leading edge
    for (uint32_t x = 1; x < m_Width; ++x)
        WritePixel(m_X + x, m_Y + m_Height - 1, cPanelLeadingEdge);

    // White trim
    for (uint32_t x = 1; x < m_Width - 1; ++x)
        WritePixel(m_X + x, m_Y + 1, cPanelTrim);
    for (uint32_t y = 1; y < m_Height - 1; ++y)
        WritePixel(m_X + 1, m_Y + y, cPanelTrim);
}
