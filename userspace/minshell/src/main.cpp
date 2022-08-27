#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "framebuffer.h"
#include "window.h"

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
    int main();
}

constexpr Pixel text_colour = pixel_from_colour(255, 255, 255);
constexpr Pixel background_colour = pixel_from_colour(0, 0, 0);
constexpr Pixel border_colour = pixel_from_colour(0, 100, 255);

constexpr size_t padding = 2;
constexpr size_t bar_height = 2;

enum View { EDITOR = 0, OUTPUT = 1 };
View view = View::EDITOR;
View last_view = view;

void draw_border();
void clear_text();

int main()
{
    // Setup UI
    draw_border();
    Keyboard keyboard(
        []() { view = View::EDITOR; }, // F1
        []() { view = View::OUTPUT; }  // F2
    );

    // Setup Lua
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // Bar
    BarWindow bar(columns - padding*2, bar_height, [](char c, unsigned int x, unsigned int y) {
        draw_char(c, y, padding + x);
    });

    // Editor
    set_background_colour(background_colour);
    #define FULL_WINDOW columns - padding*2, rows - padding*2, \
                        [](char c, unsigned int x, unsigned int y) { draw_char(c, padding + y, padding + x); }
    EditorWindow editor(FULL_WINDOW);
    OutputWindow output(FULL_WINDOW);
    Window* windows[2] = { &editor, &output };

    while(1)
    {
        // Swapped views? Redraw
        if (last_view != view) windows[view]->redraw();
        last_view = view;

        // Poll keyboard
        char key = '\0';
        keyboard.poll(&key);
        if (key == '\0') continue;

        // Update window
        windows[view]->draw(key);
    }

    lua_close(L);
    return 0;
}

void draw_border()
{
    set_background_colour(border_colour);
    set_foreground_colour(text_colour);

    for (size_t x = 0; x < width; ++x)
    {
        for (size_t y = 0; y < CHAR_HEIGHT*scale_y; ++y)
            set_pixel(border_colour, x, y);
        for (size_t y = 0; y < CHAR_HEIGHT; ++y)
            set_pixel(border_colour, x, height-y-1);
    }

    for (size_t y = 0; y < rows; ++y)
    {
        draw_char(' ', y, 0);
        draw_char(' ', y, columns-1);
    }
}
