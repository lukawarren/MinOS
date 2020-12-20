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

int main()
{
    printf("Starting window manager...", false);

    width = getFramebufferWidth();
    height = getFramebufferHeight();
    address = getFramebufferAddr();
    pitch = width * sizeof(uint32_t);

    for (uint32_t x = 0; x < width; ++x)
        for (uint32_t y = 0; y < height; ++y)
            WritePixel(x, y, GetColour((float)y / (float)height * 255.0f, (float)y / (float)height * 255.0f, (float)y / (float)height * 255.0f));

    while(1) asm("nop");
    sysexit();
    return 0;
}