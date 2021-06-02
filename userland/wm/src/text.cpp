#include "text.h"
#include "graphics.h"

Graphics::Text::Text(const char* text, const unsigned int x, const unsigned int y) : Widget(strlen(text) * CHAR_WIDTH, CHAR_HEIGHT, x, y)
{
    m_Text = text;
}

uint32_t Graphics::Text::GetPixel(const uint32_t screenX __attribute__((unused)), const uint32_t screenY __attribute__((unused))) const
{
    return 0xffffffff;
}

bool Graphics::Text::IsPixelSet(const uint32_t screenX, const uint32_t screenY) const
{
    // Convert to local space
    const uint32_t x = screenX - m_X;
    const uint32_t y = screenY - m_Y;

    // Get character
    const uint32_t nChar = x / CHAR_WIDTH;
    const uint8_t* pBitmap = GetFontFromChar(m_Text[nChar]);
    
    // Work out if pixel is inside bitap
    const uint8_t mask = 1 << (x % CHAR_WIDTH);
    return pBitmap[y % CHAR_HEIGHT] & mask;
}
