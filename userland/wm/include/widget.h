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

        inline void Redraw(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y)
        {
            m_Width = width;
            m_Height = height;
            m_X = x;
            m_Y = y;
        }

        inline void Redraw(const unsigned int x, const unsigned int y)
        {
            m_X = x;
            m_Y = y;
        }

        virtual uint32_t GetPixel(const uint32_t screenX, const uint32_t screenY) const = 0;

        virtual inline bool IsPixelSet(const uint32_t screenX __attribute__((unused)), const uint32_t screenY __attribute__((unused))) const
        {
            return
                screenX >= m_X &&
                screenY >= m_Y &&
                screenX < m_X + m_Width &&
                screenY < m_Y + m_Height;
        }
        
    protected:
        unsigned int m_Width;
        unsigned int m_Height;
        unsigned int m_X;
        unsigned int m_Y;

    };

    constexpr inline uint32_t GetColour(const uint32_t r, const uint32_t g, const uint32_t b)
    {
        const auto a = 0xff;
        return a << 24 | r << 16 | g << 8 | b;
    }
}

#endif