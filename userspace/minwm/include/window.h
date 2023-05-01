#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include "client/types.h"

const Unit bar_height = 20;
const Unit window_thickness = 3;
const Unit bar_margin_top = 1;
const Unit bar_margin_bottom = 2;

const Colour window_background = to_colour(0, 0, 0);
const Colour bar_text_colour = to_colour(255, 255, 255);
const Colour bar_start_colour = 0xff000080;
const Colour bar_end_colour = 0xff1084d0;

struct Window
{
    char title[32];
    Position position; // Position of entire window, not just framebuffer
    uint32_t* framebuffer;
    Size framebuffer_size;

    // Assume 1 window per process
    pid_t pid;

    Window(const char* title, Position position, uint32_t* framebuffer, Size size, pid_t pid)
    {
        strncpy(this->title, title, sizeof(this->title));
        this->position = position;
        this->framebuffer = framebuffer;
        this->framebuffer_size = size;
        this->pid = pid;
    }

    constexpr Position framebuffer_position() const
    {
        return position + window_thickness + Size { 0, bar_height };
    }

    constexpr Position size() const
    {
        return framebuffer_size + window_thickness * 2 + Size { 0, bar_height };
    }
};
