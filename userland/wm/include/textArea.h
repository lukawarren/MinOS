#pragma once
#ifndef TEXT_AREA_H
#define TEXT_AREA_H

#include <minlib.h>
#include "widget.h"
#include "font.h"
#include "panel.h"

namespace Graphics
{
    class TextArea: public Widget
    {
    public:
        TextArea(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned height, const uint32_t colour = 0xffffffff);
        ~TextArea();

        uint32_t GetPixel(const uint32_t x, const uint32_t y) const override;
        Pair<bool, bool> ShouldUpdate(const Input::Mouse& mouse, const uint32_t windowX, const uint32_t windowY) override;

        void AddRow(char const* text);
        void SetRow(const uint32_t row, char const* text);

    private:
        uint32_t m_colour;
        Panel m_scrollbar;
        
        Vector<String> m_vLines;
        uint32_t m_nLine;
        uint32_t m_nLinesPerView;
        uint32_t m_nColumnsPerRow;
        bool m_bScrollbar;
        
        char GetCharacterFromText(const uint32_t row, const uint32_t column) const;
    };
}

#endif
