#include "font.h"
#include "messages.h"
#include "common.h"
#include "minlib.h"
#include <string.h>
#include <stdio.h>

constexpr Unit screen_width = 640;
constexpr Unit screen_height = 480;

constexpr Unit bar_height = 36;
constexpr Unit bar_padding = 10;
constexpr Colour bar_text_colour = to_colour(255, 255, 255);

void fill_rect(Position position, Size size, Colour colour)
{
    for (Unit y = 0; y < size.y; ++y)
        for (Unit x = 0; x < size.x; ++x)
            framebuffer[(position.y + y) * screen_width + position.x + x] = colour;
}

void draw_bar(const char* title, const char* time)
{
    fill_rect({}, { screen_width, bar_height }, 0);

    draw_font("MinShell", bar_text_colour, bar_padding, bar_padding);
    draw_font_centered(title, bar_text_colour, 0, 0, screen_width, bar_height);
    draw_font(time, bar_text_colour, screen_width - bar_padding - 50, bar_padding);
}

struct Window
{
    char title[32] = "Doom Shareware";

    Position position;
    Size viewport_size;
    Size size;

    constexpr Unit thickness() const { return 5; }
    constexpr Colour background() const { return to_colour(0, 0, 0); }

    Window(Position _position, Size _viewport_size) :
        position(_position), viewport_size(_viewport_size),
        size(viewport_size + thickness()*2) {}

    void draw_frame() const
    {
        fill_rect(position, size, 0);
        return;
    }
};

int main()
{
    init_font("Gidole-Regular.sfn", 16);
    fill_rect({}, { screen_width, screen_height }, 0xffffffff);

    for(;;)
    {
        Message message;
        if (get_messages(&message, 1))
        {
            const int id = *(int*)message.data;

            if (id == CREATE_WINOW_MESSAGE)
            {
                const auto* m = (CreateWindowMessage*)&message;

                Window(
                    { 640 / 2 - m->width / 2, 480 / 2- m->height / 2 },
                    { m->width, m->height }
                ).draw_frame();

                draw_bar(m->title, "10:55");
            }
        }
    }

    free_font();
    return 0;
}
