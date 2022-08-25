#pragma once
#include <stdint.h>
#include <stddef.h>
#include "font.h"

typedef uint32_t Pixel;

constexpr size_t address = 0x30000000;
constexpr size_t width = 640;
constexpr size_t height = 480;

constexpr size_t scale_x = 1;
constexpr size_t scale_y = 2;

constexpr size_t rows = height / CHAR_HEIGHT / scale_y;
constexpr size_t columns = width / CHAR_WIDTH / scale_x;

void set_foreground_colour(const Pixel pixel);
void set_background_colour(const Pixel pixel);
void set_pixel(const Pixel pixel, const size_t x, const size_t y);
void draw_char(const char c, const size_t row, const size_t column);
void draw_string(const char* string, size_t row, size_t column);

constexpr Pixel pixel_from_colour(const uint8_t r, const uint8_t g, const uint8_t b)
{
    return 0xff000000 | Pixel(r << 16) | Pixel(g << 8) | Pixel(b);
}
