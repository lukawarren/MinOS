#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "widget.h"

namespace Graphics
{
    class Window
    {
    public:
        Window() {}
        Window(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, char const* title);
        ~Window();
    
        void Redraw();

        unsigned int m_Width;
        unsigned int m_Height;
        unsigned int m_X;
        unsigned int m_Y;

        const char* m_sTitle;

        Widget* m_pWidgets[7];
    };
}

#endif
