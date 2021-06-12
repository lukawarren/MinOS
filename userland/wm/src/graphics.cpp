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
    int swapscreenbuffer();
}

// Framebuffer
static FILE* fFramebuffer;
static uint32_t* pFramebuffer;
static uint32_t framebufferSize;
uint32_t Graphics::screenWidth;
uint32_t Graphics::screenHeight;
static uint32_t* pRowBuffer;

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

    // Row buffer
    pRowBuffer = (uint32_t*)malloc(sizeof(uint32_t) * screenWidth);

    // Make window
    auto windowWidth = 800;
    auto windowHeight = 600;
    window = new Window(windowWidth, windowHeight, screenWidth/2-windowWidth/2, screenHeight/2-windowHeight/2, "Terminal");

    printf("[Wm] Initialised\n");

    // Redraw screen
    DrawRegion(0, 0, screenWidth, screenHeight);
}

void Graphics::WriteRow(const uint32_t y, const uint32_t* pData, const size_t length, const size_t offset)
{
    assert(y < screenHeight);

    auto FastCopy = [](uint32_t* pDest, const uint32_t* pSrc, const size_t count)
    {
        asm volatile
        (
            "rep movsl\n"
            : "=S"(pSrc), "=D"(pDest)
            : "S"(pSrc), "D"(pDest), "c"(count)
            : "memory"
        );
    };

    FastCopy(&pFramebuffer[y * screenWidth + offset],                               pData, length);
    FastCopy(&pFramebuffer[y * screenWidth + offset] + screenWidth*screenHeight,    pData, length);
    //memcpy(&pFramebuffer[y * screenWidth], pData, length);
}

void Graphics::DrawRegion(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
    for (uint32_t k = 0; k < height; ++k)
    {
        const uint32_t screenY = y + k;

        for (uint32_t j = 0; j < width; ++j)
        {
            const uint32_t screenX = x + j;

            if (screenX > screenWidth || screenY > screenHeight) continue;

            bool bDrew = false;

            // Work out widgets covered by region and redraw
            for (const Widget* widget : window->m_pWidgets)
            {
                if (widget->IsCoveredByRegion(screenX, screenY) && widget->IsPixelSet(screenX, screenY))
                {
                    pRowBuffer[screenX] = widget->GetPixel(screenX, screenY);
                    bDrew = true;
                }
            }

            // Oh... we're the desktop?
            if (!bDrew)
                pRowBuffer[screenX] = 0xffff00ff;
        }

        // Blit row onto framebuffer
        WriteRow(screenY, pRowBuffer + x, width, x);
    }
    
    swapscreenbuffer();
}

void Graphics::Terminate()
{
    delete window;
    delete pRowBuffer;
    munmap((void*)pFramebuffer, framebufferSize);
    fclose(fFramebuffer);
}