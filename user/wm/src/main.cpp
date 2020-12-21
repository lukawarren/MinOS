#include "interrupts/syscall.h"
#include "stdlib.h"

int main();

static uint32_t width;
static uint32_t height;
static uint32_t address;
static uint32_t pitch;

static void WritePixel(size_t x, size_t y, uint32_t colour)
{
    size_t index = x*4 + y*pitch;
    *(uint32_t*)((uint32_t)address + index) = colour;
}

static uint32_t GetColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
{
    return a << 24 | r << 16 | g << 8 | b;
}

static void DrawRect(uint32_t x, uint32_t y, uint32_t rectWidth, uint32_t rectHeight, uint32_t colour)
{
    //uint32_t* vram = (uint32_t*)(address + x*4 + y*pitch);

    for (uint32_t i = x; i < x + rectWidth; ++i)
    {
        for (uint32_t j = y; j < y + rectHeight; ++j)
        {
            WritePixel(i, j, colour);
        }
    }
}

int main()
{
    printf("Starting window manager...", false);

    width = getFramebufferWidth();
    height = getFramebufferHeight();
    address = getFramebufferAddr();
    pitch = width * sizeof(uint32_t);

    //for (uint32_t x = 0; x < width; ++x)
    //    for (uint32_t y = 0; y < height; ++y)
    //        WritePixel(x, y, GetColour(255, 100, 50));

    DrawRect(0, 0, 20, 10, GetColour(255, 100, 50));

    while(1) asm("nop");
    sysexit();
    return 0;
}