#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "framebuffer.h"
#include "keyboard.h"

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
constexpr size_t ui_rows = rows - padding;
constexpr size_t ui_cols = columns - padding;
size_t ui_row = 0;
size_t ui_col = 0;

const char* prompt = "minshell % ";
char input[columns - padding*2] = {};
size_t input_length = 0;

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

    // Begin prompt
    draw_string(prompt, ui_row + padding, padding);
    ui_col = strlen(prompt);
    Keyboard keyboard;

    while(1)
    {
        char key = '\0';
        keyboard.poll(&key);
        if (key == '\0') continue;

        // Enter
        if (key == '\n')
        {
            // Run command...
            if (luaL_dostring(L, input) != 0)
                printf("Error: %s\n", lua_tostring(L, -1));

            // Check we haven't filled the screen
            ui_row++;
            if (ui_row >= ui_rows-padding)
            {
                ui_row = 0;
                clear_text();
            }

            // Reset state
            memset(input, '\0', sizeof(input));
            ui_col = strlen(prompt);
            draw_string(prompt, ui_row + padding, padding);
            input_length = 0;
        }

        // Backspace
        else if (key == '\b')
        {
            if (input_length == 0) continue;
            draw_char(' ', padding + ui_row, padding + ui_col - 1);
            ui_col--;
            input[input_length-1] = '\0';
            input_length--;
        }

        else
        {
            // Genuine keypress; too long? Too bad!
            if (ui_col >= ui_cols-padding) continue;
            draw_char(key, padding + ui_row, padding + ui_col);
            input[input_length] = key;
            ui_col++;
            input_length++;
        }
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

void clear_text()
{
    for (size_t y = padding * CHAR_HEIGHT; y < height - padding * CHAR_HEIGHT - 1; ++y)
        for (size_t x = padding * CHAR_WIDTH; x < width - padding * CHAR_HEIGHT - 1; ++x)
            set_pixel(background_colour, x, y);
}