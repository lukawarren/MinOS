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

void draw_border();
void clear_text();

int main()
{
    // Setup UI
    draw_border();
    set_background_colour(background_colour);

    // Setup Lua
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // Editor
    EditorWindow editorWindow(columns - padding*2, rows - padding*2, [](char c, unsigned int x, unsigned int y) {
        draw_char(c, padding + y, padding + x);
    });

    while(1)
    {
        editorWindow.draw();
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
        for (size_t y = 0; y < CHAR_HEIGHT; ++y)
        {
            set_pixel(border_colour, x, y);
            set_pixel(border_colour, x, height-y-1);
        }
    }

    for (size_t y = 0; y < rows; ++y)
    {
        draw_char(' ', y, 0);
        draw_char(' ', y, columns-1);
    }

    const char* title = "MinOS - minshell running Lua 5.4.4";
    draw_string(title, 0, columns/2 - strlen(title)/2);
}
