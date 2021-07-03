#include "textArea.h"

constexpr uint32_t cBackgroundColour = 0xffaaaaaa;

// Text
constexpr uint32_t linePadding = 2;
constexpr uint32_t lineHeight = linePadding + CHAR_HEIGHT;
constexpr uint32_t textScale = 2;

// Scrolling
constexpr uint32_t scrollbarWidth = 20;
constexpr uint32_t scrollbarHeight = 100;

namespace Graphics
{
    TextArea::TextArea(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned height, const uint32_t colour) :
        Widget(width, height, x, y), m_colour(colour), m_scrollbar(scrollbarWidth, scrollbarHeight, width - scrollbarWidth, 100)
    {
        m_vLines.Push(MakeRowFromString("Hello world!"));
        for (int i = 0; i < 100; ++i)
        {
            char dest[128];
            sprintf(dest, "Line %d", i);
            m_vLines.Push(MakeRowFromString(dest));
        }
    }

    uint32_t TextArea::GetPixel(const uint32_t x, const uint32_t y) const
    {
        const uint32_t row = y / lineHeight;
        const uint32_t column = x / CHAR_WIDTH;
        
        // Return early if scrollbar
        const uint32_t scrollbarX = x - m_scrollbar.m_X;
        const uint32_t scrollbarY = y - m_scrollbar.m_Y;
        if (m_scrollbar.IsRowSet(y) && m_scrollbar.IsPixelSet(x, y))
            return m_scrollbar.GetPixel(scrollbarX, scrollbarY);
        
        // Return if outside text area
        const bool bLinePadding = (y % lineHeight >= CHAR_HEIGHT);
        if (bLinePadding || row >= m_vLines.Length() || column >= m_vLines[row]->Length()) return cBackgroundColour;
        
        // Get character and query font bitmap
        const char character = GetCharacterFromText(row, column);
        const uint8_t* pBitmap = GetFontFromChar(character);
        const uint8_t mask = 1 << (x % CHAR_WIDTH);
        return (pBitmap[y % lineHeight] & mask) ? m_colour : cBackgroundColour;
    }
    
    Pair<bool, bool> TextArea::ShouldUpdate(const Input::Mouse& mouse, const uint32_t windowX, const uint32_t windowY)
    {
        return { false, false };
    }
    
    Vector<char>* TextArea::MakeRowFromString(char const* string) const
    {
        const auto nChars = strlen(string);
        auto row = new Vector<char>(nChars);
        for (uint32_t i = 0; i < nChars; ++i) row->Push(new char(string[i]));
        
        return row;
    }
    
    char TextArea::GetCharacterFromText(const uint32_t row, const uint32_t column) const
    {
        return *(*m_vLines[row])[column];
    }

    TextArea::~TextArea() {}
}
