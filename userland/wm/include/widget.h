#pragma once
#ifndef WIDGET_H
#define WIDGET_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

namespace Graphics
{
    class Widget
    {
    public:

        Widget() {}

        Widget(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y) :
            m_Width(width), m_Height(height), m_X(x), m_Y(y)
        {
            m_pBitmap = (uint32_t*) malloc(sizeof(uint32_t) * width * height);
        }

        virtual ~Widget()
        {
            free(m_pBitmap);
        }
    
        // Redrawing
        virtual void Redraw(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y) = 0;

        inline void Redraw(const unsigned int x, const unsigned int y)
        {
            m_X = x;
            m_Y = y;
        }
        
        // Culling
        inline bool IsRowSet(const uint32_t screenY) const
        {
            return screenY >= m_Y && screenY < m_Y + m_Height;
        }

        virtual inline bool IsPixelSet(const uint32_t screenX, const uint32_t screenY __attribute((unused))) const
        {
            // We already know the row is set, and if we don't, we can override the function,
            // so only check X bounds.
            return screenX >= m_X && screenX < m_X + m_Width;
        }
        
        virtual inline bool AreRowsIdentical() const
        {
            return true;
        }
        
        // Drawing
        virtual uint32_t GetPixel(const uint32_t screenX, const uint32_t screenY) const = 0;
        
        inline uint32_t GetPixelFromBitmap(const uint32_t screenX, const uint32_t screenY) const
        {
            return m_pBitmap[(screenY-m_Y) * m_Width + (screenX-m_X)];
        }
        
    public:
        unsigned int m_Width;
        unsigned int m_Height;
        unsigned int m_X;
        unsigned int m_Y;
        
        uint32_t* m_pBitmap = nullptr;

    };

    constexpr inline uint32_t GetColour(const uint32_t r, const uint32_t g, const uint32_t b)
    {
        const auto a = 0xff;
        return a << 24 | r << 16 | g << 8 | b;
    }
}

#endif
