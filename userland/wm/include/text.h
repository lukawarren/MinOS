#pragma once
#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "widget.h"
#include "font.h"

namespace Graphics
{
    class Text: public Widget
    {
    public:
        Text() {}

        Text(const char* text, const unsigned int x, const unsigned int y);

        ~Text() {}

        uint32_t GetPixel(const uint32_t screenX, const uint32_t screenY) const override;

        bool IsPixelSet(const uint32_t screenX, const uint32_t screenY) const override;

    private:
        char const* m_Text;
    };
}

#endif