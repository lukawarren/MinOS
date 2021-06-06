#include "graphics.h"
#include <assert.h>

#include "window.h"
#include "widget.h"
#include "panel.h"
#include "text.h"
#include "bar.h"

extern "C"
{
    extern void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
    extern void* munmap(void* addr, size_t length);

    int getscreenwidth();
    int getscreenheight();
}

// Framebuffer
static FILE* fFramebuffer;
static uint32_t* pFramebuffer;
static uint32_t framebufferSize;
uint32_t Graphics::screenWidth;
uint32_t Graphics::screenHeight;

// Windows
Graphics::Window* window;

void Graphics::Init()
{
    // Get screen dimensions
    screenWidth = getscreenwidth();
    screenHeight = getscreenheight();

    // Open framebuffer
    fFramebuffer = fopen("/dev/fb", "w+");

    // Get size
    struct stat framebufferStat;
    fstat(fFramebuffer->_file, &framebufferStat);
    framebufferSize = framebufferStat.st_size;

    // Map into memory
    pFramebuffer = (uint32_t*) mmap(NULL, framebufferSize, PROT_WRITE | PROT_READ, MAP_SHARED, fFramebuffer->_file, 0);
   
    // Clear framebuffer
    for (uint32_t i = 0; i < screenWidth*screenHeight; ++i)
        pFramebuffer[i] = 0;

    // Make window
    auto windowWidth = 400;
    auto windowHeight = 200;
    window = new Window(windowWidth, windowHeight, screenWidth/2-windowWidth/2, screenHeight/2-windowHeight/2, "Terminal");

    printf("[Wm] Initialised\n");

    // Redraw screen
    DrawRegion(0, 0, screenWidth, screenHeight);
}

void Graphics::WritePixel(const uint32_t x, const uint32_t y, const uint32_t colour)
{
    assert(x < screenWidth && y < screenHeight);
    pFramebuffer[y * screenWidth + x] = colour;
}

void Graphics::DrawRegion(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
    for (uint32_t j = 0; j < width; ++j)
    {
        for (uint32_t k = 0; k < height; ++k)
        {
            const uint32_t screenX = x + j;
            const uint32_t screenY = y + k;

            if (screenX > screenWidth || screenY > screenHeight) continue;

            bool bDrew = false;

            // Work out widgets covered by region and redraw
            for (const Widget* widget : window->m_pWidgets)
            {
                if (widget->IsCoveredByRegion(screenX, screenY) && widget->IsPixelSet(screenX, screenY))
                {
                    WritePixel(screenX, screenY, widget->GetPixel(screenX, screenY));
                    bDrew = true;
                }
            }

            // Oh... we're the desktop?
            if (!bDrew)
                WritePixel(screenX, screenY, 0xffff00ff);
        }
    }
}

void Graphics::Terminate()
{
    delete window;
    munmap((void*)pFramebuffer, framebufferSize);
    fclose(fFramebuffer);
}