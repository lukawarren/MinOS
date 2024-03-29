#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int main(void)
{
    // Open lua
    lua_State *L = luaL_newstate();

    // Load the libraries
    luaL_openlibs(L);

    // Run lua
    luaL_dostring(L, "print \"Hello world from Lua!\"");
    luaL_dostring(L, "print (\"Lua just told me 5 * 4 = \" .. 5*4 .. \"!\")");

    // Close lua
    lua_close (L);

    volatile int hang = 1;
    while(hang) {}
    return 0;
}
