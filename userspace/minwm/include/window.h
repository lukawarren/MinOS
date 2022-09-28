#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "common.h"

const Unit window_thickness = 5;
const Colour window_background = to_colour(0, 0, 0);

struct Window
{
    char title[32];
    Position position; // Position of entire window, not just framebuffer
    uint32_t* framebuffer;
    Size framebuffer_size;

    Window(const char* title, Position position, uint32_t* framebuffer, Size size)
    {
        strncpy(this->title, title, sizeof(this->title));
        this->position = position;
        this->framebuffer = framebuffer;
        this->framebuffer_size = size;
    }

    constexpr Position framebuffer_position() const
    {
        return position + window_thickness;
    }

    constexpr Position size() const
    {
        return framebuffer_size + window_thickness * 2;
    }
};
