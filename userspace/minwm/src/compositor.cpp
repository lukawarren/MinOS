#include "compositor.h"
#include "font.h"

constexpr Colour background = to_colour(255, 255, 255);
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
    // For "skipping ahead" to the other side
    const Position offset = window->framebuffer_size + window_thickness;
    const Position offset_x = { offset.x, 0 };
    const Position offset_y = { 0, offset.y };

    // For avoiding overdraw with vertical bars
    const Size overdraw_size = { 0, window_thickness };

    // Horizontal; top
    fill_rect(
        window->position,
        Size { window->size().x, window_thickness },
        window_background
    );

    // Horizontal; bottom
    fill_rect(
        window->position + offset_y,
        Size { window->size().x, window_thickness },
        window_background
    );

    // Vertical; left
    fill_rect(
        window->position + overdraw_size,
        Size { window_thickness, window->size().y } - overdraw_size,
        window_background
    );

    // Vertical; right
    fill_rect(
        window->position + overdraw_size + offset_x,
        Size { window_thickness, window->size().y } - overdraw_size,
        window_background
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
