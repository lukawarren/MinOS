#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "minlib.h"

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

constexpr size_t max_input_length = 4096;

void print_prompt();

int main()
{
    // Init Lua
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // Open console
    FILE* console = fopen("uart", "r");
    assert(console);

    // Input buffer
    char buffer[max_input_length] = {};
    size_t buffer_len = 0;

    // Disable stdout buffering
    setbuf(stdout, NULL);
    print_prompt();

    for (;;)
    {
        // Read input while it lasts
        for (;;)
        {
            char input;
            if (read(fileno(console), &input, 1) == 0) break;
            buffer[buffer_len++] = input;
            printf("%c", input);
        }

        // Backspace
        if (buffer[buffer_len-1] == 8 || buffer[buffer_len-1] == 127)
        {
            if (buffer_len > 1)
            {
                printf("%c", 8);
                printf(" ");
                printf("%c", 8);
                buffer_len -= 2;
                buffer[buffer_len + 1] = '\0';
                buffer[buffer_len + 0] = '\0';
            }
            else
            {
                buffer_len = 0;
                buffer[0] = '\0';
            }
        }

        // New line
        if (buffer_len != 0 && (buffer[buffer_len-1] == '\r' || buffer[buffer_len-1] == '\n'))
        {
            // Run Lua :)
            printf("\n");
            if (luaL_dostring(L, buffer) != 0)
                printf("%s\n", lua_tostring(L, -1));

            // Clear prompt
            memset(buffer, '\0', buffer_len);
            buffer_len = 0;
            print_prompt();
        }

    }

    lua_close(L);
    fclose(console);
    assert(false);
    return 0;
}

void print_prompt()
{
    printf("[minshell] ");
}