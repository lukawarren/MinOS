#include "text.h"

Graphics::Text::Text(char const* text, const uint32_t colour)
{
    m_Text = (char*) malloc(sizeof(char) * strlen(text));
    strcpy(m_Text, text);
    m_Width = strlen(text) * CHAR_WIDTH;
    m_Height = CHAR_HEIGHT;
    m_Colour = colour;
}

Graphics::Text::Text(char const* text, const unsigned int x, const unsigned int y, const uint32_t colour) : Widget(strlen(text) * CHAR_WIDTH, CHAR_HEIGHT, x, y)
{
    m_Text = (char*) malloc(sizeof(char) * strlen(text));
    strcpy(m_Text, text);
    m_Colour = colour;
}

uint32_t Graphics::Text::GetPixel(const uint32_t, const uint32_t) const
{
    return m_Colour;
}

bool Graphics::Text::IsPixelSet(const uint32_t windowSpaceX, const uint32_t windowSpaceY) const
{
    if (windowSpaceX < m_X) return false;
    if (windowSpaceY < m_Y) return false;
    if (windowSpaceX >= m_X + m_Width) return false;
    if (windowSpaceY >= m_Y + m_Height) return false;

    // Convert to local space
    const uint32_t x = windowSpaceX - m_X;
    const uint32_t y = windowSpaceY - m_Y;

    // Get character
    const uint32_t nChar = x / CHAR_WIDTH;
    const uint8_t* pBitmap = GetFontFromChar(m_Text[nChar]);
    
    // Work out if pixel is inside bitmap
    const uint8_t mask = 1 << (x % CHAR_WIDTH);
    return pBitmap[y % CHAR_HEIGHT] & mask;
}

Graphics::Text::~Text()
{
    free(m_Text);
}
