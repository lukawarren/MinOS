#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "framebuffer.h"

extern "C" { int main(); }

constexpr Pixel text_colour = pixel_from_colour(255, 255, 255);
constexpr Pixel background_colour = pixel_from_colour(0, 0, 0);
constexpr Pixel border_colour = pixel_from_colour(0, 100, 255);

void draw_border();

int main()
{
    draw_border();
    set_background_colour(background_colour);



    volatile int hang = 1;
    while(hang) {}
    return 0;
}

void draw_border()
{
    set_background_colour(border_colour);
    set_foreground_colour(text_colour);

    for (size_t x = 0; x < width; ++x)
    {
        for (size_t y = 0; y < CHAR_HEIGHT; ++y)
        {
            set_pixel(border_colour, x, y);
            set_pixel(border_colour, x, height-y-1);
        }
    }

    for (size_t y = 0; y < rows; ++y)
    {
        draw_char(' ', y, 0);
        draw_char(' ', y, columns-1);
    }

    const char* title = "MinOS - minshell";
    draw_string(title, 0, columns/2 - strlen(title)/2);
}