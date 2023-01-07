#include <minlib.h>
#include <client/window.h>
#include <client/drawing.h>

// Window
constexpr Vector2<Unit> screen_size = { 300, 300 };
Window window("Snake", screen_size);

// Snake
constexpr int tiles_per_side = 5;
constexpr Unit tile_size = screen_size.x / tiles_per_side;
Vector<Position> body;

void draw();

int main()
{
    body.push(new Position(0, 0));
    draw();

    volatile int hang = 1;
    while (hang) {}
    return 0;
}

void draw()
{
    // Tiles - TODO: avoid overdraw
    for (int y = 0; y < tiles_per_side; ++y)
    {
        for (int x = 0; x < tiles_per_side; ++x)
        {
            draw_panel(
                window.framebuffer,
                screen_size.x,
                { x * tile_size, y * tile_size },
                { tile_size, tile_size }
            );
        }
    }

    // Snake
    body.for_each([](Position* p)
    {
        draw_panel(
            window.framebuffer,
            screen_size.x,
            *p * tile_size,
            { tile_size, tile_size },
            true,
            0xff00ff00
        );
    });
}