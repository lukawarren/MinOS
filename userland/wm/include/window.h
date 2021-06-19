#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <minlib.h>
#include "widget.h"
#include "mouse.h"

namespace Graphics
{
    class Window
    {
    public:
        Window() {}
        Window(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, char const* title);
        ~Window();
    
        void Redraw();
        Pair<bool, Pair<uint32_t, uint32_t>> ShouldUpdate(const Input::Mouse& mouse, const uint32_t screenWidth, const uint32_t screenHeight);

        unsigned int m_Width;
        unsigned int m_Height;
        unsigned int m_X;
        unsigned int m_Y;

        const char* m_sTitle;

        Vector<Widget> m_vWidgets;
    
    private:
        bool m_bDragged;
    };
}

#endif
