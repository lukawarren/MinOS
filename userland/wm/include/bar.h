#pragma once
#ifndef BAR_H
#define BAR_H

#include <stdint.h>
#include <stddef.h>

#include "widget.h"

namespace Graphics
{
    class Bar: public Widget
    {
    public:
        Bar() {}

        Bar(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y) :
            Widget(width, height, x, y) {}

        ~Bar() {}

        uint32_t GetPixel(const uint32_t screenX, const uint32_t screenY) const override;
    };
}

#endif