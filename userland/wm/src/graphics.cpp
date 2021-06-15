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

inline void Graphics::WriteRow(const uint32_t y, const uint32_t* pData, size_t length, const size_t offset)
{
    const uint32_t* dest = &pFramebuffer[y * screenWidth + offset];
    asm volatile
    (
        "rep movsl\n"
        : "+S"(pData), "+D"(dest), "+c"(length)::"memory"
    );
}

void Graphics::DrawRegion(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
    // For each row...
    for (uint32_t k = 0; k < height; ++k)
    {
        const uint32_t screenY = y + k;

        // Fill row with desktop
        for (uint32_t j = 0; j < width; ++j) pRowBuffer[x +j] = 0xffff00ff;

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
                    pRowBuffer[screenX] = widget->GetPixelFromBitmap(screenX, screenY);
                }
            }
        }

        // Blit row onto framebuffer
        WriteRow(screenY, pRowBuffer + x, width, x);
    }
}

void Graphics::Terminate()
{
    delete window;
    delete pRowBuffer;
    munmap((void*)pFramebuffer, framebufferSize);
    fclose(fFramebuffer);
}
