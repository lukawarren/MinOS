#include "compositor.h"
#include "font.h"
#include <assert.h>

constexpr Colour background = 0xffc0c0c0;
Colour* framebuffer = (Colour*) 0x30000000;

Compositor::Compositor(const Size screen_size)
{
    this->screen_size = screen_size;
    blit_background();
}

void Compositor::display_bar(const char* message)
{
    const Unit height = 32;
    const Colour colour = to_colour(0, 0, 0);
    fill_rect({ 0, 0 }, { screen_size.x, height }, colour);
    draw_font_centered(message, 0xffffffff, 0, 0, screen_size.x, height);
}

void Compositor::display_window(Window* window)
{
    blit_background();
    blit_window(window);
}

void Compositor::redraw_window(Window* window)
{
    blit_window_framebuffer(window);
}

void Compositor::blit_background()
{
    fill_rect({}, screen_size, background);
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
    assert(window_thickness == 2);

    // Top left "corner"
    fill_rect(
        window->position + Size { 1, 0 },
        Size { window->size().x-2, 1 },
        0xffdfdfdf
    );
    fill_rect(
        window->position,
        Size { 1, window->size().y-1 },
        0xffdfdfdf
    );

    // Bottom right "corner"
    fill_rect(
        window->position + Size { 0, window->size().y-1 },
        Size { window->size().x, 1 },
        0xff000000
    );
    fill_rect(
        window->position + Size { window->size().x-1, 0 },
        Size { 1, window->size().y-1 },
        0xff000000
    );

    // Inner top left "corner"
    fill_rect(
        window->position + Size { 2, 1 },
        Size { window->size().x-3, 1 },
        0xffffffff
    );
    fill_rect(
        window->position + Size { 1, 1 },
        Size { 1, window->size().y-2 },
        0xffffffff
    );

    // Inner bottom right "corner"
    fill_rect(
        window->position + Size { 1, window->size().y-2 },
        Size { window->size().x-2, 1 },
        0xff808080
    );
    fill_rect(
        window->position + Size { window->size().x-2, 1 },
        Size { 1, window->size().y-2 },
        0xff808080
    );
}

void Compositor::blit_window(Window* window)
{
    blit_window_framebuffer(window);
    blit_window_border(window);
}

void Compositor::fill_rect(Position position, Size size, Colour colour)
{
    for (Unit y = 0; y < size.y; ++y)
        for (Unit x = 0; x < size.x; ++x)
            framebuffer[(position.y + y) * screen_size.x + position.x + x] = colour;
}
