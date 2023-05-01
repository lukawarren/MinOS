#include "compositor.h"
#include "client/drawing.h"
#include "font.h"
#include <assert.h>

constexpr Colour background = to_colour(50, 50, 50);
Colour* framebuffer = (Colour*) 0x30000000;

Compositor::Compositor(const Size screen_size, const Unit bottom_bar_height)
{
    this->screen_size = screen_size;
    this->bottom_bar_height = bottom_bar_height;
    blit_background();
}

void Compositor::display_bar(const char* message)
{
    auto height = bottom_bar_height;
    draw_panel(framebuffer, screen_size.x, { 0, screen_size.y - height }, { screen_size.x, height });
    draw_font_centered(message, 0xff111111, 0, screen_size.y - height, screen_size.x, height);
}

void Compositor::display_window(Window* window)
{
    blit_window(window);
}

void Compositor::redraw_window(Window* window)
{
    blit_window(window);
}

void Compositor::redraw_window_framebuffer(Window* window)
{
    blit_window_framebuffer(window);
}

void Compositor::redraw_window_bar(Window* window)
{
    blit_window_bar(window);
}

void Compositor::blit_background()
{
    fill_rect(framebuffer, screen_size.x, {}, screen_size, background);
}

void Compositor::blit_window_framebuffer(Window* window)
{
    const auto pos = window->framebuffer_position();

    for (Unit row = 0; row < window->framebuffer_size.y; ++row)
    {
        const Unit y_offset = (pos.y + row) * screen_size.x;
        const Unit local_offset = row * window->framebuffer_size.x;
        const Unit size = window->framebuffer_size.x;

        memcpy(
            framebuffer + y_offset + pos.x,
            window->framebuffer + local_offset,
            size * sizeof(framebuffer[0])
        );
    }
}

void Compositor::blit_window_border(Window* window)
{
    draw_panel(framebuffer, screen_size.x, window->position, window->size(), false);
}

void Compositor::blit_window_bar(Window* window)
{
    // Bar background
    fill_rect(
        framebuffer,
        screen_size.x,
        window->position + window_thickness,
        Size { window->size().x - window_thickness*2, bar_margin_top } ,
        panel_background_colour
    );
    draw_gradient(
        framebuffer,
        screen_size.x,
        window->position + window_thickness + Size { 0, bar_margin_top },
        Size { window->size().x - window_thickness*2, bar_height - bar_margin_top - bar_margin_bottom } ,
        bar_start_colour,
        bar_end_colour
    );
    fill_rect(
        framebuffer,
        screen_size.x,
        window->position + window_thickness + Size { 0, bar_height - bar_margin_bottom },
        Size { window->size().x - window_thickness*2, bar_margin_bottom } ,
        panel_background_colour
    );

    // Bar text
    draw_font_centered(
        window->title,
        bar_text_colour,
        window->position.x,
        window->position.y + 2,
        window->size().x,
        bar_height
    );
}

void Compositor::blit_window(Window* window)
{
    blit_window_framebuffer(window);
    blit_window_border(window);
    blit_window_bar(window);
}
