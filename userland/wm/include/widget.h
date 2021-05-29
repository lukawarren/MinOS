#pragma once
#ifndef WIDGET_H
#define WIDGET_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

namespace Graphics
{
    class Widget
    {
    public:

        Widget() {}

        Widget(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y) :
            m_Width(width), m_Height(height), m_X(x), m_Y(y) {}

        virtual ~Widget() {}
        
        virtual void Draw(const uint32_t screenX, const uint32_t screenY) const = 0;

        bool IsCoveredByRegion(const uint32_t x, const uint32_t y) const
        {
            return
                x >= m_X &&
                y >= m_Y &&
                x <= m_X + m_Width &&
                y <= m_Y + m_Height;
        }

    protected:
        unsigned int m_Width;
        unsigned int m_Height;
        unsigned int m_X;
        unsigned int m_Y;

    };
}

#endif