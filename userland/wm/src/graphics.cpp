#include "graphics.h"
#include <assert.h>

#include "widget.h"
#include "panel.h"
#include "bar.h"

#define WIDTH 1024
#define HEIGHT 768

// Framebuffer
static FILE* fFramebuffer;
static uint32_t* pFramebuffer;

// Widgets
static Graphics::Widget* pWidgets[2];

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

    Panel* panel = (Panel*) malloc(sizeof(Panel));
    printf("Panel location: 0x%X\n", (unsigned int)panel);
    asm("xchg %bx, %bx");
    *panel = Panel(600, 400, 100, 100);
    pWidgets[0] = panel;

    Bar* bar = (Bar*) malloc(sizeof(Bar));
    *bar = Bar(600, 20, 100, 50);
    pWidgets[1] = bar;

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

            // Work out widgets covered by region and redraw
            for (const Widget* widget : pWidgets)
            {
                if (widget->IsCoveredByRegion(screenX, screenY)) widget->Draw(screenX, screenY);
            }
        }
    }
}

void Graphics::Terminate()
{
    //for (const Widget* widget : pWidgets) delete widget;
    fclose(fFramebuffer);
}