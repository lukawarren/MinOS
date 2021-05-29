#pragma once
#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>
#include <stddef.h>

#include "widget.h"

namespace Graphics
{
    class Panel: public Widget
    {
    public:
        Panel() {}

        Panel(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y) :
            Widget(width, height, x, y) {}

        ~Panel() {}

        void Draw(const uint32_t screenX, const uint32_t screenY) const override;
    };
}

#endif