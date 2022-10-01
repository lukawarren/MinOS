#pragma once
#include "common.h"
#include "window.h"

class Compositor
{
public:
    Compositor(const Size screen_size);

    void display_window(Window* window);
    void redraw_window(Window* window);
    void move_window(Window* window, Position position);

private:
    void blit_window_framebuffer(Window* window);
    void blit_window_border(Window* window);
    void blit_window(Window* window);

    void fill_rect(Position position, Size size, Colour colour);

    Size screen_size;
};