#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <minlib.h>
#include "widget.h"
#include "text.h"

namespace Graphics
{
    class Button: public Widget
    {
    public:
        Button(char const* sText, const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height) :
            Widget(width, height, x, y),
            m_text(sText, width / 2 - strlen(sText) * CHAR_WIDTH / 2, height / 2 - CHAR_HEIGHT / 2) {}

        ~Button() {}
        
        uint32_t GetPixel(const uint32_t x, const uint32_t y) const override;
        
        virtual bool ShouldUpdate(const Input::Mouse& mouse, const uint32_t windowX, const uint32_t windowY) override;
        
        bool m_bActive = false;
        
    private:
        Text m_text;
    };
}

#endif
