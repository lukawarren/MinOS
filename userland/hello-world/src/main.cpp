#include "font.h"
#include "memory/memory.h"

#define SCREEN_WIDTH 1024

uint32_t* pFramebuffer;

int main();
void DrawChar(const char c, const uint32_t x, const uint32_t y);

int main()
{
    pFramebuffer = (uint32_t*)FRAMEBUFFER_OFFSET;

    DrawChar('H', CHAR_WIDTH * 0,  0);
    DrawChar('e', CHAR_WIDTH * 1,  0);
    DrawChar('l', CHAR_WIDTH * 2,  0);
    DrawChar('l', CHAR_WIDTH * 3,  0);
    DrawChar('o', CHAR_WIDTH * 4,  0);
    DrawChar(' ', CHAR_WIDTH * 5,  0);
    DrawChar('f', CHAR_WIDTH * 6,  0);
    DrawChar('r', CHAR_WIDTH * 7,  0);
    DrawChar('o', CHAR_WIDTH * 8,  0);
    DrawChar('m', CHAR_WIDTH * 9,  0);
    DrawChar(' ', CHAR_WIDTH * 10, 0);
    DrawChar('u', CHAR_WIDTH * 11, 0);
    DrawChar('s', CHAR_WIDTH * 12, 0);
    DrawChar('e', CHAR_WIDTH * 13, 0);
    DrawChar('r', CHAR_WIDTH * 14, 0);
    DrawChar('l', CHAR_WIDTH * 15, 0);
    DrawChar('a', CHAR_WIDTH * 16, 0);
    DrawChar('n', CHAR_WIDTH * 17, 0);
    DrawChar('d', CHAR_WIDTH * 18, 0);
    DrawChar('!', CHAR_WIDTH * 19, 0);
    while (1) asm("nop");
}

void DrawChar(char c, const uint32_t x, const uint32_t y)
{
    const uint8_t* bitmap = GetFontFromChar(c);

    for (int w = 0; w < CHAR_WIDTH; ++w)
    {
        for (int h = 0; h < CHAR_HEIGHT*CHAR_HEIGHT_SCALE; ++h)
        {
            uint8_t mask = 1 << (w);

            size_t xPos = x + w;
            size_t yPos = y + h;
            
            if (bitmap[h/CHAR_HEIGHT_SCALE] & mask) *(pFramebuffer + yPos*SCREEN_WIDTH+xPos) = 0xffffffff;
            else *(pFramebuffer + yPos*SCREEN_WIDTH+xPos) = 0;
        }
    }
}