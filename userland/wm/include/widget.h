#pragma once
#ifndef WIDGET_H
#define WIDGET_H

#include <minlib.h>
#include "mouse.h"

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
        void Render()
        {
            if (m_pBitmap != nullptr) free(m_pBitmap);
            m_pBitmap = (uint32_t*) malloc(sizeof(uint32_t) * m_Width * m_Height);
                
            for (uint32_t x = 0; x < m_Width; ++x)
                for (uint32_t y = 0; y < m_Height; ++y)
                    m_pBitmap[y * m_Width + x] = GetPixel(x, y);
        }
        
        // Culling
        inline bool IsRowSet(const uint32_t windowSpaceY) const
        {
            return windowSpaceY >= m_Y && windowSpaceY < m_Y + m_Height;
        }

        virtual inline bool IsPixelSet(const uint32_t windowSpaceX, const uint32_t windowSpaceY __attribute((unused))) const
        {
            // We already know the row is set, and if we don't, we can override the function,
            // so only check X bounds.
            return windowSpaceX >= m_X && windowSpaceX < m_X + m_Width;
        }
        
        virtual inline bool AreRowsIdentical() const
        {
            return true;
        }
        
        // Drawing
        inline uint32_t GetPixelFromBitmap(const uint32_t windowSpaceX, const uint32_t windowSpaceY) const
        {
            return m_pBitmap[(windowSpaceY-m_Y) * m_Width + (windowSpaceX-m_X)];
        }
        
        // Input
        virtual bool ShouldUpdate(const Input::Mouse&, const uint32_t, const uint32_t)
        {
            return false;
        }
    
    protected:
        virtual uint32_t GetPixel(const uint32_t x, const uint32_t y) const = 0;
        
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
