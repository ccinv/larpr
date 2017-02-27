/*
** The MIT License (MIT)
** Copyright (c) <2016> <avaicode>
*/


#include <lua.h>

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_larpr(lua_State* L);
int larpr_init(lua_State* L);
int larpr_require(lua_State* L, const char* module);
int larpr_setppath(lua_State* L, const char* path);

#ifdef __cplusplus
}
#endif
