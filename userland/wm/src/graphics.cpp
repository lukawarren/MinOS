#include "graphics.h"
#include <assert.h>

#include "widget.h"
#include "panel.h"
#include "text.h"
#include "bar.h"

extern "C"
{
    extern void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
}

// Framebuffer
static FILE* fFramebuffer;
static uint32_t* pFramebuffer;

// Widgets
static Graphics::Widget* pWidgets[4];

void Graphics::Init()
{
    // Open framebuffer
    fFramebuffer = fopen("/dev/fb", "w+");

    // Get size
    struct stat framebufferStat;
    fstat(fFramebuffer->_file, &framebufferStat);
    const uint32_t framebufferSize = framebufferStat.st_mode;

    // Map into memory
    pFramebuffer = (uint32_t*) mmap(NULL, framebufferSize, PROT_WRITE | PROT_READ, MAP_SHARED, fFramebuffer->_file, 0);
   
    // Clear framebuffer
    for (uint32_t i = 0; i < WIDTH*HEIGHT; ++i)
        pFramebuffer[i] = 0;

    pWidgets[0] = new Panel(600, 400, WIDTH/2-600/2, HEIGHT/2-400/2);
    pWidgets[1] = new Bar(600, 20,WIDTH/2-600/2, HEIGHT/2-400/2-20);
    pWidgets[2] = new Text("Terminal", WIDTH/2-600/2+5, HEIGHT/2-400/2-20/2-CHAR_HEIGHT/2);
    pWidgets[3] = new Text("colonel@minos /home $", WIDTH/2-600/2+5, HEIGHT/2-400/2+CHAR_HEIGHT/2);

    // Redraw screen
    DrawRegion(0, 0, WIDTH, HEIGHT);
}

void Graphics::WritePixel(const uint32_t x, const uint32_t y, const uint32_t colour)
{
    assert(x < WIDTH && y < HEIGHT);
    pFramebuffer[y * WIDTH + x] = colour;
}

void Graphics::DrawRegion(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
    for (uint32_t j = 0; j < width; ++j)
    {
        for (uint32_t k = 0; k < height; ++k)
        {
            const uint32_t screenX = x + j;
            const uint32_t screenY = y + k;

            if (screenX > WIDTH || screenY > HEIGHT) continue;

            bool bDrew = false;

            // Work out widgets covered by region and redraw
            for (const Widget* widget : pWidgets)
            {
                if (widget->IsCoveredByRegion(screenX, screenY) && widget->IsPixelSet(screenX, screenY))
                {
                    WritePixel(screenX, screenY, widget->GetPixel(screenX, screenY));
                    bDrew = true;
                }
            }

            // Oh... we're the desktop?
            if (!bDrew)
                WritePixel(screenX, screenY, 0);
        }
    }
}

void Graphics::Terminate()
{
    for (const Widget* widget : pWidgets) delete widget;
    fclose(fFramebuffer);
}