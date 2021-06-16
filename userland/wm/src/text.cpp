#include "text.h"

Graphics::Text::Text(const char* text)
{
    m_Text = text;
    m_Width = strlen(text) * CHAR_WIDTH;
    m_Height = CHAR_HEIGHT;
}

Graphics::Text::Text(const char* text, const unsigned int x, const unsigned int y) : Widget(strlen(text) * CHAR_WIDTH, CHAR_HEIGHT, x, y)
{
    m_Text = text;
}

uint32_t Graphics::Text::GetPixel(const uint32_t, const uint32_t) const
{
    return 0xffffffff;
}

bool Graphics::Text::IsPixelSet(const uint32_t screenX, const uint32_t screenY) const
{
    if (screenX < m_X) return false;
    if (screenY < m_Y) return false;
    if (screenX >= m_X + m_Width) return false;
    if (screenY >= m_Y + m_Height) return false;
    
    // Convert to local space
    const uint32_t x = screenX - m_X;
    const uint32_t y = screenY - m_Y;

    // Get character
    const uint32_t nChar = x / CHAR_WIDTH;
    const uint8_t* pBitmap = GetFontFromChar(m_Text[nChar]);
    
    // Work out if pixel is inside bitmap
    const uint8_t mask = 1 << (x % CHAR_WIDTH);
    return pBitmap[y % CHAR_HEIGHT] & mask;
}

