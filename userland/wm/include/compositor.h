#pragma once
#ifndef GRPAHICS_H
#define GRAPHICS_H

#include <minlib.h>
#include "window.h"
#include "mouse.h"
#include "bitmap.h"

namespace Graphics
{
    class Compositor
    {
    public:
        Compositor();
        ~Compositor();
        
        void DrawRegion(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
        void UpdateAndDrawMouse(Input::Mouse& mouse);
        
        void MoveWindow(Window* pWindow, const uint32_t x, const uint32_t y);
        Window* GetWindowForPID(const uint32_t pid) const;

    private:
        inline void WriteRow(const uint32_t y, const uint32_t* pData, size_t length, const size_t offset)
        {
            const uint32_t* dest = &m_pFramebuffer[y * m_screenWidth + offset];
            asm volatile
            (
                "rep movsl\n"
                : "+S"(pData), "+D"(dest), "+c"(length)::"memory"
            );
        }

        Bitmap m_bDesktop;

    public:
        FILE*        m_fFramebuffer;
        uint32_t*    m_pFramebuffer;
        uint32_t     m_framebufferSize;
        uint32_t*    m_pRowBuffer;
    
        uint32_t m_screenWidth;
        uint32_t m_screenHeight;
    
        Vector<Window> m_vWindows;
    };
}

#endif
