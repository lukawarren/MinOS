#include "button.h"
#include "panel.h"

uint32_t Graphics::Button::GetPixel(const uint32_t x, const uint32_t y) const
{
    // If within text, return early
    if (m_text.IsPixelSet(x, y)) return m_text.m_Colour;
    
    // Right leading edge
    if (y > 0 && x == m_Width-1) return cPanelLeadingEdge;

    // Bottom leading edge
    if (x > 0 && y == m_Height-1) return cPanelLeadingEdge;

    // White trim
    if (x < m_Width  && y == 0) return cPanelTrim;
    if (y < m_Height && x == 0) return cPanelTrim;

    return m_bActive ? cPanelLeadingEdge : cPanelBackground;
}

bool Graphics::Button::ShouldUpdate(const Input::Mouse& mouse, const uint32_t windowX, const uint32_t windowY)
{
    const unsigned int mouseX = (unsigned int)(mouse.m_sState.x);
    const unsigned int mouseY = (unsigned int)(mouse.m_sState.y);
    
    const unsigned int widgetX = m_X + windowX;
    const unsigned int widgetY = m_Y + windowY;
    
    if (mouseX >= widgetX && mouseY >= widgetY &&
        mouseX < widgetX + m_Width &&
        mouseY < widgetY + m_Height)
    {
        // When first hovered over
        if (!m_bActive)
        {
            m_bActive = true;
            return true;
        }
    }
    
    // First time "un-hovered"
    else if (m_bActive)
    {
        m_bActive = false;
        return true;
    }
    
    return false;
}
