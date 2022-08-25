#include "framebuffer.h"
#include "font.h"

Pixel* pixels = (Pixel*)address;
Pixel foreground = pixel_from_colour(255, 255, 255);
Pixel background = pixel_from_colour(0, 0, 0);

void set_foreground_colour(const Pixel pixel) { foreground = pixel; }
void set_background_colour(const Pixel pixel) { background = pixel; }

void set_pixel(const Pixel pixel, const size_t x, const size_t y)
{
    pixels[y * width + x] = pixel;
}

void draw_char(char c, size_t row, size_t column)
{
    const uint8_t* bitmap = get_char_from_font(c);
    const size_t x = column * CHAR_WIDTH * scale_x;
    const size_t y = row * CHAR_HEIGHT * scale_y;

    for (size_t ly = 0; ly < CHAR_HEIGHT*scale_y; ++ly)
    {
        for (size_t lx = 0; lx < CHAR_WIDTH*scale_x; ++lx)
        {
            const auto colour = (bitmap[ly/scale_y] & (1 << lx/scale_x)) ? foreground : background;
            set_pixel(colour, x + lx, y + ly);
        }
    }
}

void draw_string(const char* string, size_t row, size_t column)
{
    while (*string != '\0')
        draw_char(*string++, row, column++);
}