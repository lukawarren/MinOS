#pragma once
#include <stdint.h>

extern "C"
{
    void init_font(const char* path, int font_size);
    void draw_font(const char* message, uint32_t colour, unsigned int x, unsigned int y);
    void draw_font_centered(const char* message, uint32_t colour, unsigned int x, unsigned int y, unsigned int boundsX, unsigned int boundsY);
    void free_font();
}