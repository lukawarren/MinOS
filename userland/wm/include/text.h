#pragma once
#ifndef TEXT_H
#define TEXT_H

#include <minlib.h>
#include "widget.h"
#include "font.h"

namespace Graphics
{
    class Text: public Widget
    {
    public:
        Text(const uint32_t colour = 0xff000000) : m_Colour(colour) {}
        
        Text(char const* text, const uint32_t colour = 0xff000000);
        
        Text(char const* text, const unsigned int x, const unsigned int y, const uint32_t colour = 0xff000000);

        ~Text();

        uint32_t GetPixel(const uint32_t x, const uint32_t y) const override;

        bool IsPixelSet(const uint32_t windowSpaceX, const uint32_t windowSpaceY) const override;

        void SetText(char const* text);

        uint32_t m_Colour;

    private:
        char* m_Text;
    };
}

#endif
