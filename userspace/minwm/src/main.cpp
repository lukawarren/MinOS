#include "font.h"
#include "messages.h"
#include "common.h"
#include "minlib.h"
#include "vector.h"
#include "window.h"
#include <string.h>
#include <stdio.h>
#include <sys/time.h> // DO NOT STAGE

constexpr Unit screen_width = 640;
constexpr Unit screen_height = 480;
constexpr Size screen_size = { screen_width, screen_height };
constexpr Colour background = to_colour(255, 255, 255);

void fill_rect(Position position, Size size, Colour colour)
{
    for (Unit y = 0; y < size.y; ++y)
        for (Unit x = 0; x < size.x; ++x)
            framebuffer[(position.y + y) * screen_width + position.x + x] = colour;
}

void poll_messages(Vector<Window>& windows);
void blit_window_framebuffer(Window* window);
void blit_window_border(Window* window);
void blit_window(Window* window);
void move_window(Window* window, Position position);

int main()
{
    printf("[minwm] starting...\n");
    init_font("Gidole-Regular.sfn", 16);
    fill_rect({}, { screen_width, screen_height }, background);

    Vector<Window> windows;

    for(;;)
    {
        // Deal with messages
        poll_messages(windows);

        // Re-draw window contents
        windows.for_each([](auto* w) {
            blit_window_framebuffer(w);
        });

        struct timeval val;
        gettimeofday(&val, NULL);
        static int bob = 0;
        if (val.tv_sec == 1 && !bob)
        {
            bob = true;
            move_window(windows[0], { 100, 100 });
        }
    }

    free_font();
    return 0;
}

void poll_messages(Vector<Window>& windows)
{
    Message message;
    while (get_messages(&message, 1))
    {
        const int id = *(int*)message.data;

        if (id == CREATE_WINOW_MESSAGE)
        {
            const auto* m = (CreateWindowMessage*)&message;

            const Size size = { m->width, m->height };
            const Position position = screen_size/2 - size/2;

            windows.push(new Window(m->title, position, m->framebuffer, size));
            blit_window(windows[windows.size()-1]);
        }

        else printf("[minwm] unknown message %d\n", id);
    }
}

void blit_window_framebuffer(Window* window)
{
    const auto pos = window->framebuffer_position();

    for (Unit row = 0; row < window->framebuffer_size.y; ++row)
    {
        const Unit y_offset = (pos.y + row) * screen_width;
        const Unit local_offset = row * window->framebuffer_size.x;
        const Unit size = window->framebuffer_size.x;

        memcpy(
            framebuffer + y_offset + pos.x,
            window->framebuffer + local_offset,
            size * sizeof(framebuffer[0])
        );
    }
}

void blit_window_border(Window* window)
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

void blit_window(Window* window)
{
    blit_window_framebuffer(window);
    blit_window_border(window);
}

void move_window(Window* window, Position position)
{
    fill_rect(window->position, window->size(), background);
    window->position = position;
    blit_window(window);
}