#include "interrupts/syscall.h"
#include "stdlib.h"

int main();

int main()
{
    printf("Starting window manager...", false);

    uint32_t width = getFramebufferWidth();
    uint32_t height = getFramebufferHeight();
    uint32_t address = getFramebufferAddr();
    uint32_t pitch = width * sizeof(uint32_t);

    auto WritePixel = [&](size_t x, size_t y, uint32_t colour)
    {
        size_t index = x*4 + y*pitch;
        *(uint32_t*)((uint32_t)address + index) = colour;
    };
    asm("xchg %bx, %bx");
    for (uint32_t x = 0; x < 10; ++x)
        for (uint32_t y = 0; y < 10; ++y)
            WritePixel(x, y, 0xffffffff);
    
    while(1) asm("nop");
    sysexit();

    return 0;
}