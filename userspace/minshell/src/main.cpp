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
    void __cxa_pure_virtual() { assert(false); }
}

// Styling
constexpr Pixel text_colour = pixel_from_colour(255, 255, 255);
constexpr Pixel background_colour = pixel_from_colour(0, 0, 0);
constexpr Pixel border_colour = pixel_from_colour(0, 100, 255);
constexpr size_t padding = 2;
constexpr size_t bar_height = 2;

// Views
enum View { EDITOR = 0, OUTPUT = 1 };
View view = View::EDITOR;
View last_view = view;

// Lua
lua_State *L;
bool lua_should_run = false;

void draw_border();

int main()
{
    // Setup UI
    printf("[minshell] starting...\n");
    draw_border();
    Keyboard keyboard(
        []() { view = View::EDITOR;   }, // F1
        []() { view = View::OUTPUT;   }, // F2
        []() { lua_should_run = true; }  // F3
    );

    // Bar
    BarWindow bar(columns - padding*2, bar_height, [](char c, unsigned int x, unsigned int y) {
        draw_char(c, y, padding + x);
    });

    set_background_colour(background_colour);
    #define FULL_WINDOW columns - padding*2, rows - padding*2, \
                        [](char c, unsigned int x, unsigned int y) { draw_char(c, padding + y, padding + x); }

    // Editor
    EditorWindow editor(FULL_WINDOW);

    // Output
    OutputWindow output(FULL_WINDOW);
    Window* windows[2] = { &editor, &output };

    // Setup Lua...
    L = luaL_newstate();
    luaL_openlibs(L);

    // ...redirecting stdout (TODO: support multiple arguments)
    lua_pushlightuserdata(L, output.output);
    lua_pushlightuserdata(L, &output.output_size);
    lua_pushcclosure(L, [](lua_State* L)
    {
        auto* out = (char*) lua_topointer(L, lua_upvalueindex(1));
        auto* size = (size_t*) lua_topointer(L, lua_upvalueindex(2));
        int n_args = lua_gettop(L);
        if (n_args < 1) return 0;

        auto* arg_as_string = luaL_tolstring(L, 1, nullptr);
        strncpy(out, arg_as_string, *size);
        return 0;
    }, 2);
    lua_setglobal(L, "print");

    printf("[minshell] running...\n");
    while(1)
    {
        // Run Lua if need be
        if (lua_should_run)
        {
            if (luaL_dostring(L, editor.input) != 0)
                strncpy(output.output, lua_tostring(L, -1), output.output_size);

            lua_should_run = false;
            windows[view]->redraw();
        }

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
