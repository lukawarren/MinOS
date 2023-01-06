#pragma once
#include "common.h"

/*
    Contains code designed to be used by "clients" (i.e. the windows themselves).
    Also used by the compositor.
*/

constexpr uint32_t panel_background_colour = 0xffc0c0c0;

void fill_rect(
    uint32_t* framebuffer,
    const Unit framebuffer_width,
    const Position position,
    const Size size,
    const Colour colour
)
{
    for (Unit y = 0; y < size.y; ++y)
        for (Unit x = 0; x < size.x; ++x)
            framebuffer[(position.y + y) * framebuffer_width + position.x + x] = colour;
}

void draw_gradient(
    uint32_t* framebuffer,
    const Unit framebuffer_width,
    const Position position,
    const Size size,
    const Colour start,
    const Colour end
)
{
    const auto lerp = [](uint8_t a, uint8_t b, float f)
    {
        return uint8_t((float)a + f * ((float)b - (float)a));
    };

    for (Unit y = 0; y < size.y; ++y)
    {
        for (Unit x = 0; x < size.x; ++x)
        {
            const float f = (float)x / (float)size.x;
            Colour colour = to_colour(
                lerp((start & 0xff0000) >> 16, (end & 0xff0000) >> 16, f),
                lerp((start & 0x00ff00) >> 8, (end & 0x00ff00) >> 8, f),
                lerp(start & 0x0000ff, end & 0x0000ff, f)
            );
            framebuffer[(position.y + y) * framebuffer_width + position.x + x] = colour;
        }
    }
}

void draw_panel(
    uint32_t* framebuffer,
    const Unit framebuffer_width,
    const Position position,
    const Size size,
    const bool fill = true
)
{
    // Top left "corner"
    fill_rect(
        framebuffer,
        framebuffer_width,
        position + Size { 1, 0 },
        Size { size.x-2, 1 },
        0xffdfdfdf
    );
    fill_rect(
        framebuffer,
        framebuffer_width,
        position,
        Size { 1, size.y-1 },
        0xffdfdfdf
    );

    // Bottom right "corner"
    fill_rect(
        framebuffer,
        framebuffer_width,
        position + Size { 0, size.y-1 },
        Size { size.x, 1 },
        0xff000000
    );
    fill_rect(
        framebuffer,
        framebuffer_width,
        position + Size { size.x-1, 0 },
        Size { 1, size.y-1 },
        0xff000000
    );

    // Inner top left "corner"
    fill_rect(
        framebuffer,
        framebuffer_width,
        position + Size { 2, 1 },
        Size { size.x-3, 1 },
        0xffffffff
    );
    fill_rect(
        framebuffer,
        framebuffer_width,
        position + Size { 1, 1 },
        Size { 1, size.y-2 },
        0xffffffff
    );

    // Inner bottom right "corner"
    fill_rect(
        framebuffer,
        framebuffer_width,
        position + Size { 1, size.y-2 },
        Size { size.x-2, 1 },
        0xff808080
    );
    fill_rect(
        framebuffer,
        framebuffer_width,
        position + Size { size.x-2, 1 },
        Size { 1, size.y-2 },
        0xff808080
    );

    if (fill)
        fill_rect(
            framebuffer,
            framebuffer_width,
            position + Size { 2, 2 },
            Size { size.x - 4, size.y - 4 },
            panel_background_colour
        );
    else
    {
        fill_rect(
            framebuffer,
            framebuffer_width,
            position + Size { 2, 2 },
            Size { size.x - 4, 1 },
            panel_background_colour
        );
        fill_rect(
            framebuffer,
            framebuffer_width,
            position + Size { 2, size.y - 3 },
            Size { size.x - 4, 1 },
            panel_background_colour
        );
        fill_rect(
            framebuffer,
            framebuffer_width,
            position + Size { 2, 3 },
            Size { 1, size.y - 6 },
            panel_background_colour
        );
        fill_rect(
            framebuffer,
            framebuffer_width,
            position + Size { size.x - 3, 3 },
            Size { 1, size.y - 6 },
            panel_background_colour
        );
    }
}