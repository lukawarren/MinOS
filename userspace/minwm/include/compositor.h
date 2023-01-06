#pragma once
#include "common.h"
#include "window.h"

class Compositor
{
public:
    Compositor(const Size screen_size);

    void display_bar(const char* message);
    void display_window(Window* window);
    void redraw_window(Window* window);

private:
    void blit_background();

    void blit_window_framebuffer(Window* window);
    void blit_window_border(Window* window);
    void blit_window(Window* window);

    Size screen_size;
};