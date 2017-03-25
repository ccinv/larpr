/*
** The MIT License (MIT)
** Copyright (c) <2016> <avaicode>
*/

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "larpr.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

const char* checkexe(void) {
#ifdef _WIN32
    static char ret[MAX_PATH];
    GetModuleFileName(NULL, ret, MAX_PATH);
#else
    static char ret[PATH_MAX];
    readlink("/proc/self/exe", ret, PATH_MAX);
#endif
    return ret;
}

int main(int argc, char* argv[]) {
    lua_State* L = luaL_newstate();
    int i;

    luaL_openlibs(L);
    lua_newtable(L);
    for (i = 0; i < argc; ++i) {
        lua_pushnumber(L, i);
        lua_pushstring(L, argv[i]);
        lua_rawset(L, -3);
    }
    lua_setglobal(L, "arg");
    larpr_init(L, checkexe());
    lua_close(L);
    return 0;
}
