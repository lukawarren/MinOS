#pragma once
#include <stdint.h>

extern "C"
{
    void init_font(const char* path, int font_size);
    void draw_font(const char* message, uint32_t colour, unsigned int x, unsigned int y);
    void free_font();
}