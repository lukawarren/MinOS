#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
int main();
}

constexpr size_t framebuffer_address = 0x30000000;
constexpr size_t framebuffer_width = 640;
constexpr size_t framebuffer_height = 480;

int main()
{
    uint32_t* framebuffer = (uint32_t*)framebuffer_address;
    for (size_t y = 0; y < framebuffer_height; ++y)
    {
        for (size_t x = 0; x < framebuffer_width; ++x)
        {
            const size_t r = x;
            const size_t g = y;
            const size_t b = 0;
            framebuffer[y * framebuffer_width + x] = 0xff000000 | (r << 16) | (g << 8) | b;

        }
    }

    // Open lua
    lua_State *L = luaL_newstate();

    // Load the libraries
    luaL_openlibs(L);

    // Run lua
    luaL_dostring(L, "print \"Hello world!\"");

    // Close lua
    lua_close (L);

    volatile int hang = 1;
    while(hang) {}
    return 0;
}
