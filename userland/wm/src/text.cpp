#include "text.h"
#include "graphics.h"

Graphics::Text::Text(const char* text, const unsigned int x, const unsigned int y) : Widget(strlen(text) * CHAR_WIDTH, CHAR_HEIGHT, x, y)
{
    m_Text = text;

    const uint32_t width = strlen(text) * CHAR_WIDTH;
    const uint32_t height = CHAR_HEIGHT;
    
    for (uint32_t _x = 0; _x < width; ++_x)
        for (uint32_t _y = 0; _y < height; ++_y)
            m_pBitmap[_y * width + _x] = GetPixel(x + _x, y + _y);
}

uint32_t Graphics::Text::GetPixel(const uint32_t screenX __attribute__((unused)), const uint32_t screenY __attribute__((unused))) const
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

void Graphics::Text::Redraw(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y)
{
    // Resize and fill buffer if width changed
    if (width != m_Width)
    {
        free(m_pBitmap);
        m_pBitmap = (uint32_t*) malloc(sizeof(uint32_t) * width * height);
        
        for (uint32_t _x = 0; _x < width; ++_x)
            for (uint32_t _y = 0; _y < height; ++_y)
                m_pBitmap[_y * width + _x] = GetPixel(x + _x, y + _y);
    }
    
    m_Width = width;
    m_Height = height;
    m_X = x;
    m_Y = y;
}
