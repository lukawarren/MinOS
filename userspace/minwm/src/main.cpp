#include "font.h"
#include "common.h"

constexpr Colour window_background = to_colour(255, 255, 255);
constexpr Colour window_bar = to_colour(0, 0, 0);

void fill_rect(Position position, Size size, Colour colour)
{
    for (uint y = 0; y < size.y; ++y)
        for (uint x = 0; x < size.x; ++x)
            framebuffer[(position.y + y) * 640 + position.x + x] = colour;
}

int main()
{
    Position window_pos = { 10, 10 };
    Position window_size = { 500, 300 };
    uint bar_height = 32;
    uint bar_padding = 5;

    fill_rect(window_pos + Position { 0, bar_height }, window_size, window_background);
    fill_rect(window_pos, { window_size.x, bar_height }, window_bar);

    init_font("Gidole-Regular.sfn", 16);
    draw_font("Doom Shareware", window_background, window_pos.x + bar_padding, window_pos.y + bar_padding);
    free_font();

    volatile int hang = 1;
    while(hang) { hang = 1; }
    return 0;
}
