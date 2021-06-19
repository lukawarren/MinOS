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

    // Redraw screen on both buffers
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

        for (uint32_t nWindow = 0; nWindow < m_vWindows.Length(); ++nWindow)
        {
            Window* window = m_vWindows[nWindow];
            
            for (size_t nWidget = 0; nWidget < window->m_vWidgets.Length(); ++nWidget)
            {
                auto widget = window->m_vWidgets[nWidget];
                
                // Cull invalid rows early
                if (!widget->IsRowSet(screenY - window->m_Y)) continue;
                
                // Copy set pixels from widget buffer into framebuffer
                for (uint32_t j = 0; j < width; ++j)
                {
                    const uint32_t screenX = x + j;

                    // If pixel is set, set pixel
                    if (widget->IsPixelSet(screenX - window->m_X, screenY - window->m_Y))
                    {
                        m_pRowBuffer[screenX] = widget->GetPixelFromBitmap(screenX - window->m_X, screenY - window->m_Y);
                    }
                }
            }
        }

        // Blit row onto framebuffer
        WriteRow(screenY, m_pRowBuffer + x, width, x);
    }
}

void Graphics::Compositor::DrawMouse(Input::Mouse& mouse)
{
    auto oldState = mouse.m_sState;
    auto state = mouse.UpdateState(m_screenWidth, m_screenHeight);
    
    if (oldState.x == state.x && oldState.y == state.y &&
        oldState.bLeftButton == state.bLeftButton && oldState.bRightButton == state.bRightButton) return;
    
    // Draw over old position
    DrawRegion(oldState.x, oldState.y, MOUSE_BITMAP_WIDTH, MOUSE_BITMAP_HEIGHT);
    
    for (uint32_t x = 0; x < MOUSE_BITMAP_WIDTH; ++x)
    {
        for (uint32_t y = 0; y < MOUSE_BITMAP_HEIGHT; ++y)
        {
            const uint32_t positionX = state.x + x;
            const uint32_t positionY = state.y + y;
            
            if (positionX < m_screenWidth && positionY < m_screenHeight &&
                MOUSE_BITMAP[y * MOUSE_BITMAP_WIDTH + x] == '1')
                m_pFramebuffer[positionY * m_screenWidth + positionX] = state.bLeftButton ? 0xff999999 : 0xffffffff;
        }
    }
}

void Graphics::Compositor::MoveWindow(Window* pWindow, const uint32_t x, const uint32_t y)
{
    // Store old position
    uint32_t oldX = pWindow->m_X;
    uint32_t oldY = pWindow->m_Y;
    uint32_t oldEndX = oldX + pWindow->m_Width;
    uint32_t oldEndY = oldY + pWindow->m_Height + 20;
    
    // Move window
    pWindow->m_X = x;
    pWindow->m_Y = y;

    // Work out region of change
    uint32_t chosenX = MIN(oldX, pWindow->m_X);
    uint32_t chosenY = MIN(oldY, pWindow->m_Y);
    uint32_t chosenEndX = MAX(oldEndX, pWindow->m_Width + pWindow->m_X);
    uint32_t chosenEndY = MAX(oldEndY, pWindow->m_Height + pWindow->m_Y);

    DrawRegion(chosenX, chosenY, chosenEndX - chosenX, chosenEndY - chosenY);
}

Graphics::Window* Graphics::Compositor::GetWindowForPID(const uint32_t pid) const
{
    for (uint32_t i = 0; i < m_vWindows.Length(); ++i)
    {
        if (m_vWindows[i]->m_PID == pid) return m_vWindows[i];
    }

    return nullptr;
}

Graphics::Compositor::~Compositor()
{
    delete m_pRowBuffer;
    munmap((void*)m_pFramebuffer, m_framebufferSize);
    fclose(m_fFramebuffer);
}
