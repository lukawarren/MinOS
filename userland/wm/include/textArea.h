#pragma once
#ifndef TEXT_AREA_H
#define TEXT_AREA_H

#include <minlib.h>
#include "widget.h"
#include "font.h"

namespace Graphics
{
    class TextArea: public Widget
    {
    public:
        TextArea(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned height, const uint32_t colour = 0xffffffff);
        ~TextArea();

        uint32_t GetPixel(const uint32_t x, const uint32_t y) const override;

    private:
        uint32_t m_Colour;
    };
}

#endif
