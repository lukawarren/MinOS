#include "textArea.h"

Graphics::TextArea::TextArea(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned height, const uint32_t colour) :
    Widget(width, height, x, y), m_Colour(colour) {}

uint32_t Graphics::TextArea::GetPixel(const uint32_t x, const uint32_t y) const
{
    return 0xffffffff;
}

Graphics::TextArea::~TextArea() {}
