#include "textArea.h"

constexpr uint32_t cBackgroundColour = 0xffaaaaaa;

// Text
constexpr uint32_t linePadding = 4;
constexpr uint32_t lineHeight = linePadding + CHAR_HEIGHT;
constexpr uint32_t textScale = 2;

// Scrolling
constexpr uint32_t scrollbarWidth = 20;
constexpr uint32_t scrollbarHeight = 100;

namespace Graphics
{
    TextArea::TextArea(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned height, const uint32_t colour) :
        Widget(width, height, x, y), m_colour(colour), m_scrollbar(scrollbarWidth, scrollbarHeight, width - scrollbarWidth, 0)
    {
        m_nLinesPerView = height / lineHeight;
        
        const auto AddLine = [&](const char* string)
        {
            m_vLines.Push(new String(string));
            m_bScrollbar = m_vLines.Length() > m_nLinesPerView;
        };
        
        for (int i = 0; i < 10; ++i)
        {
            AddLine("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras hendrerit");
            AddLine("neque eget eros elementum placerat. Etiam blandit tellus sapien, vitae");
            AddLine("tempus ligula imperdiet eget. Aenean accumsan pharetra est, rutrum");
            AddLine("sodales nisl vestibulum vel. Fusce lacinia tortor sit amet eleifend");
            AddLine("semper. Morbi placerat ipsum a tellus venenatis, vitae dapibus magna");
            AddLine("porttitor. Sed dignissim eros sapien, in fringilla odio molestie in.");
            AddLine("Aenean nec euismod nibh. Etiam nibh augue, condimentum eget dui eget,");
            AddLine("posuere lobortis quam. Phasellus a tristique tellus. In egestas mi a");
            AddLine("dignissim sodales. Donec vel mollis felis. Maecenas at dictum mi, in");
            AddLine("tristique lorem. Donec et enim mauris. Proin sit amet lorem turpis. Duis");
            AddLine("vehicula nisi eu ex tempor, at consequat magna euismod.");
            AddLine("");
            AddLine("");
        }
        m_nLine = 0;
    }

    uint32_t TextArea::GetPixel(const uint32_t x, const uint32_t y) const
    {
        const uint32_t row = y / lineHeight + m_nLine;
        const uint32_t column = x / CHAR_WIDTH;
        
        // Return early if scrollbar
        const uint32_t scrollbarX = x - m_scrollbar.m_X;
        const uint32_t scrollbarY = y - m_scrollbar.m_Y;
        if (m_scrollbar.IsRowSet(y) && m_scrollbar.IsPixelSet(x, y) && m_bScrollbar)
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
        if (!m_bScrollbar) return { false, false };
        
        // Get mouse position
        const unsigned int mouseX = (unsigned int)(mouse.m_sState.x);
        
        // Transform scrollbar position to window space
        const uint32_t scrollbarX = m_scrollbar.m_X + m_X + windowX;
        
        // If over scrollbar area (X-wise)...
        if (mouse.m_sState.bLeftButton && mouseX >= scrollbarX && mouseX < scrollbarX + m_scrollbar.m_Width)
        {
            // Stop scrollbar going too high and convert to widget space
            int cappedMouseY = mouse.m_sState.y - (int)windowY - (int)m_Y; 
            cappedMouseY = MAX(cappedMouseY, int(m_scrollbar.m_Height / 2));
            
            // Move scrollbar accordingly
            m_scrollbar.m_Y = cappedMouseY - m_scrollbar.m_Height / 2;
            
            // Stop scrollbar going too low
            m_scrollbar.m_Y = MIN(m_scrollbar.m_Y, m_Height - m_scrollbar.m_Height);
            
            // Work out progress to end of bar
            const uint32_t progress = m_scrollbar.m_Y * 100 / (m_Height - m_scrollbar.m_Height);
            m_nLine = (m_vLines.Length() - m_nLinesPerView) * progress / 100;
            
            return { true, false };
        }
        
        return { false, false };
    }
    
    char TextArea::GetCharacterFromText(const uint32_t row, const uint32_t column) const
    {
        return (*m_vLines[row])[column];
    }

    TextArea::~TextArea() {}
}
