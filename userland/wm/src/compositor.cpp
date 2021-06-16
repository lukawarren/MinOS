#include <minlib.h>
#include "compositor.h"
#include "window.h"
#include "widget.h"
#include "panel.h"
#include "text.h"
#include "bar.h"

Graphics::Compositor::Compositor()
{
    // Get screen dimensions
    m_screenWidth = getscreenwidth();
    m_screenHeight = getscreenheight();

    // Open framebuffer
    m_fFramebuffer = fopen("/dev/fb", "w+");

    // Get size
    struct stat framebufferStat;
    fstat(m_fFramebuffer->_file, &framebufferStat);
    m_framebufferSize = framebufferStat.st_size;

    // Map into memory
    m_pFramebuffer = (uint32_t*) mmap(NULL, m_framebufferSize, PROT_WRITE | PROT_READ, MAP_SHARED, m_fFramebuffer->_file, 0);
   
    // Clear framebuffer
    for (uint32_t i = 0; i < m_screenWidth*m_screenHeight; ++i)
        m_pFramebuffer[i] = 0;

    // Row buffer
    m_pRowBuffer = (uint32_t*)malloc(sizeof(uint32_t) * m_screenWidth);

    printf("[Wm] Initialised\n");
    
    window = new Window(100, 100, 0, 0, "bob");

    // Redraw screen
    DrawRegion(0, 0, m_screenWidth, m_screenHeight);
}

void Graphics::Compositor::DrawRegion(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
    // For each row...
    for (uint32_t k = 0; k < height; ++k)
    {
        const uint32_t screenY = y + k;

        // Fill row with desktop
        for (uint32_t j = 0; j < width; ++j) m_pRowBuffer[x +j] = 0xffff00ff;

        for (const Widget* widget : window->m_pWidgets)
        {
            // Cull invalid rows early
            if (!widget->IsRowSet(screenY)) continue;
            
            // Copy set pixels from widget buffer into framebuffer
            for (uint32_t j = 0; j < width; ++j)
            {
                const uint32_t screenX = x + j;

                // If pixel is set, set pixel
                if (widget->IsPixelSet(screenX, screenY))
                {
                    m_pRowBuffer[screenX] = widget->GetPixelFromBitmap(screenX, screenY);
                }
            }
        }

        // Blit row onto framebuffer
        WriteRow(screenY, m_pRowBuffer + x, width, x);
    }
}

void Graphics::Compositor::DrawMouse(Input::Mouse& mouse)
{
    auto oldPosition = mouse.m_sPosition;
    auto position = mouse.UpdatePosition(m_screenWidth, m_screenHeight);
    
    // Draw over old position
    DrawRegion(oldPosition.x, oldPosition.y, MOUSE_BITMAP_WIDTH, MOUSE_BITMAP_HEIGHT);
    
    for (uint32_t x = 0; x < MOUSE_BITMAP_WIDTH; ++x)
    {
        for (uint32_t y = 0; y < MOUSE_BITMAP_HEIGHT; ++y)
        {
            const uint32_t positionX = position.x + x;
            const uint32_t positionY = position.y + y;
            
            if (positionX < m_screenWidth && positionY < m_screenHeight &&
                MOUSE_BITMAP[y * MOUSE_BITMAP_WIDTH + x] == '1')
                m_pFramebuffer[positionY * m_screenWidth + positionX] = 0xffffffff;
        }
    }
}

Graphics::Compositor::~Compositor()
{
    delete m_pRowBuffer;
    munmap((void*)m_pFramebuffer, m_framebufferSize);
    fclose(m_fFramebuffer);
}
