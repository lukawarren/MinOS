#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "framebuffer.h"

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

const char* prompt = "minshell %  ";
char input[columns - padding*2] = {};
size_t input_length = 0;
bool shift = false;

char get_key();
void draw_border();

int main()
{
    // Setup UI
    draw_border();
    set_background_colour(background_colour);

    // Begin prompt
    draw_string(prompt, ui_row + padding, padding);
    ui_col = strlen(prompt);

    while(1)
    {
        char key = get_key();
        if (key == '\0') continue;

        // Enter
        if (key == '\n')
        {
            // Run command
            lua_State *L = luaL_newstate();
            luaL_openlibs(L);
            luaL_dostring(L, input);
            lua_close(L);

            // Reset state
            memset(input, '\0', sizeof(input));
            ui_row++;
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

    const char* title = "MinOS - minshell";
    draw_string(title, 0, columns/2 - strlen(title)/2);
}

char get_key()
{
    // Read from kernel
    char scancode;
    read(3, &scancode, sizeof(char));
    if (scancode == 0) return '\0';

    // Shifts
    if (scancode == 42 || scancode == -86)
    {
        shift = !shift;
        return '\0';
    }

    static char const* topNumbers =   "!\"#$%^&*(";
    static char const* numbers =      "123456789";
    static char const* qwertzuiop =   "qwertyuiopQWERTYUIOP";
    static char const* asdfghjkl =    "asdfghjklASDFGHJKL";
    static char const* yxcvbnm =      "zxcvbnmZXCVBNM";

    // Standard characters
    if (scancode >= 0x2 && scancode <= 0xa)
    {
        if (shift) return topNumbers[scancode - 0x2];
        else return numbers[scancode - 0x2];
    }
    else if(scancode >= 0x10 && scancode <= 0x1b) return qwertzuiop[scancode - 0x10 + shift*10];
    else if(scancode >= 0x1E && scancode <= 0x26) return asdfghjkl[scancode - 0x1E + shift*9];
    else if(scancode >= 0x2C && scancode <= 0x32) return yxcvbnm[scancode - 0x2C + shift*7];

    // Special characters
    else if (scancode == 57) return ' ';
    else if (scancode == 51) return shift ? '<' : ',';
    else if (scancode == 52) return shift ? '>' : '.';
    else if (scancode == 28) return '\n';
    else if (scancode == 14) return '\b';
    return '\0';
}