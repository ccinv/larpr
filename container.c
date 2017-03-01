/*
** The MIT License (MIT)
** Copyright (c) <2016> <avaicode>
*/

#include <lua.h>
#include <lauxlib.h>

#include "larpr.h"

int main(int argc, char* argv[]) {
    lua_State* L = luaL_newstate();
    int i;

    luaL_openlibs(L);
    lua_newtable(L);
    for (i = 0; i < argc; ++i) {
        lua_pushstring(L, argv[i]);
        lua_seti(L, -2, i);
    }
    larpr_init(L, argv[0]);
    lua_close(L);
    return 0;
}
