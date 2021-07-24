#pragma once
#ifndef BAR_H
#define BAR_H

#include <minlib.h>
#include "widget.h"

namespace Graphics
{
    class Bar: public Widget
    {
    public:
        Bar() {}

        Bar(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const uint32_t colour = 0xffffffff) :
            Widget(width, height, x, y), r(uint8_t(colour >> 16)), g(uint8_t(colour >> 8)), b(uint8_t(colour)) {}

        ~Bar() {}
        
        uint32_t GetPixel(const uint32_t x, const uint32_t y) const override;
    
        void SetColour(const uint32_t colour);

    private:
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
}

#endif
