#include "font.h"
#include "common.h"

void fill_rect(Position position, Size size, Colour colour)
{
    for (Unit y = 0; y < size.y; ++y)
        for (Unit x = 0; x < size.x; ++x)
            framebuffer[(position.y + y) * 640 + position.x + x] = colour;
}

struct Window
{
    char title[32] = "Doom Shareware";

    Position position;
    Size size;

    Window(Position _position, Size _size) : position(_position), size(_size) {}

    constexpr Unit bar_height() const { return 32; }
    constexpr Colour background() const { return to_colour(100, 100, 100); }
    constexpr Colour bar_background() const { return to_colour(0, 0, 0); }
    constexpr Colour bar_foreground() const { return to_colour(255, 255, 255); }

    void draw_bar() const
    {
        fill_rect(position + Position { 0, bar_height() }, size, background());
        fill_rect(position, { size.x, bar_height() }, bar_background());
        draw_font_centered(
            title,
            bar_foreground(),
            position.x,
            position.y,
            size.x,
            bar_height()
        );
    }
};

int main()
{
    Size screen_size = { 640, 480 };
    init_font("Gidole-Regular.sfn", 16);
    fill_rect({}, screen_size, 0xffffffff);

    Window({100, 100}, {320, 240}).draw_bar();
    
    free_font();

    volatile int hang = 1;
    while(hang) { hang = 1; }
    return 0;
}
