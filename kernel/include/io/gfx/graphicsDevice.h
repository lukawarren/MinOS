#pragma once
#ifndef GRAPHICS_DEVICE_H
#define GRAPHICS_DEVICE_H

#include <stdint.h>
#include <stddef.h>

#include "kstdlib.h"

namespace Framebuffer
{
    class GraphicsDevice
    {
    public:
        GraphicsDevice() {}
        ~GraphicsDevice() {}

        virtual void SwapBuffers() = 0;

        uint32_t m_Address = 0;
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_Pitch;
        uint32_t m_Size;

        inline void SetPixel(uint32_t x, uint32_t y, uint32_t colour)
        {
            *((uint32_t*)m_Address + y*m_Width + x) = colour;
        }
    };
}

#endif