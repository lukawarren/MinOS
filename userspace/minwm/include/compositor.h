#pragma once
#include "client/types.h"
#include "window.h"

class Compositor
{
public:
    Compositor(const Size screen_size, const Unit bottom_bar_height);

    void display_bar(const char* message);
    void display_window(Window* window);

    void redraw_window(Window* window);
    void redraw_window_framebuffer(Window* window);
    void redraw_window_bar(Window* window);

    void blit_background();
private:
    void blit_window_framebuffer(Window* window);
    void blit_window_border(Window* window);
    void blit_window_bar(Window* window);
    void blit_window(Window* window);

    Size screen_size;
    Unit bottom_bar_height;
};