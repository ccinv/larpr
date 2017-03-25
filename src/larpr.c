/*
** Lua ARhieve
** The MIT License (MIT)
** Copyright (c) <2016> <avaicode>
*/

#define LUA_LIB
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "deps/compat-5.3/c-api/compat-5.3.h"

#include <stdio.h>
#include <string.h>

#ifdef USE_EXTERN_MINIZ
    #define EXTERN_MINIZ_NAME "miniz"
#else
    #define MINIZ_HEADER_FILE_ONLY
    #include "deps/miniz/miniz.c"
#endif

#define LARPR_VERSION "larpr 1.0rc"

#ifdef _WIN32
    #define LUA_PPATH_DEFAULT   ".\\?.lar"
#else
    #define LUA_PPATH_DEFAULT   "./?.lar"
#endif

#define INIT_FIELD    "_init"
#define LUA_PATH_SEP  ";"
#define LUA_PATH_MARK "?"
#define PATH_SEP      "/"

#define LARPR_NAME    "larpr"
#define LAR_FIELD     "_LARS"
#define CURRENT_FIELD "_CNT"
#define PPATH_FIELD   "ppath"
#define VERSION_FIELD "VERSION"

#define INFO_OFFEST 17
#define INFO_ID_LEN 6
#define INFO_MAGIC "<larpr"
#define INFO_BLOCK "interal"

#if LUA_VERSION_NUM < 502
    #define LUA_SEARCHERS "loaders"
    #define lua53_load(x, y, z, a, b) lua_load(x, y, z, a)
#else
    #define LUA_SEARCHERS "searchers"
    #define lua53_load lua_load
#endif

/* lminiz reader begins */

#ifdef USE_EXTERN_MINIZ

static int Lzip_read_file_panic(lua_State* L) {
    luaL_error(L, "miniz not found");
    return 0;
}

#define MINIZ_EXTERN_INIT_BINDING(x) \
    static int L##x(lua_State* L){ \
        int ori = lua_gettop(L); \
        int i = 1; \
        luaL_requiref(L, EXTERN_MINIZ_NAME, Lzip_read_file_panic, 0); \
        lua_getfield(L, -1, #x); \
        for (; i <= ori; ++i) \
            lua_pushvalue(L, i); \
        lua_call(L, ori, LUA_MULTRET); \
        return lua_gettop(L) - ori - 1; \
    }

MINIZ_EXTERN_INIT_BINDING(zip_read_file)
MINIZ_EXTERN_INIT_BINDING(zip_read_mem)
#undef MINIZ_EXTERN_INIT_BINDING

#define MINIZ_EXTERN_MEMBER_BINDING(x) \
    static int Lreader_##x(lua_State* L){ \
        int ori = lua_gettop(L); \
        int i = 1; \
        lua_getfield(L, 1, #x); \
        for (; i <= ori; ++i) \
            lua_pushvalue(L, i); \
        lua_call(L, ori, LUA_MULTRET); \
        return lua_gettop(L) - ori; \
    }

MINIZ_EXTERN_MEMBER_BINDING(extract)
MINIZ_EXTERN_MEMBER_BINDING(get_num_files)
MINIZ_EXTERN_MEMBER_BINDING(get_filename)
MINIZ_EXTERN_MEMBER_BINDING(is_file_a_directory)
MINIZ_EXTERN_MEMBER_BINDING(reader_close)
#undef MINIZ_EXTERN_MEMBER_BINDING

#else

static int Lzip_read_file(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);
    mz_uint32 flags = (mz_uint32)luaL_optinteger(L, 2, 0);
    mz_zip_archive* za = lua_newuserdata(L, sizeof(mz_zip_archive));
    memset(za, 0, sizeof(mz_zip_archive));
    if (!mz_zip_reader_init_file(za, filename, flags))
        return 0;
    return 1;
}

static int Lzip_read_mem(lua_State* L) {
    const char* data = (const char*)lua_touserdata(L, 1);
    int size = lua_tointeger(L, 2);
    mz_uint32 flags = (mz_uint32)luaL_optinteger(L, 3, 0);
    mz_zip_archive* za = lua_newuserdata(L, sizeof(mz_zip_archive));
    memset(za, 0, sizeof(mz_zip_archive));
    if (!mz_zip_reader_init_mem(za, data, size, flags))
        return 0;
    return 1;
}

static size_t Lwriter(void* ud, mz_uint64 file_ofs, const void* p, size_t n) {
    luaL_addlstring((luaL_Buffer*)ud, p, n);
    return n;
}

static int Lreader_extract(lua_State* L) {
    mz_zip_archive* za = lua_touserdata(L, 1);
    mz_uint flags = (mz_uint)luaL_optinteger(L, 3, 0);
    int type = lua_type(L, 2);
    luaL_Buffer b;
    mz_bool result = 0;
    luaL_buffinit(L, &b);
    if (type == LUA_TSTRING)
        result = mz_zip_reader_extract_file_to_callback(za,
                 lua_tostring(L, 2),
                 Lwriter, &b, flags);
    else if (type == LUA_TNUMBER)
        result = mz_zip_reader_extract_to_callback(za,
                 (mz_uint)lua_tointeger(L, 2) - 1,
                 Lwriter, &b, flags);
    luaL_pushresult(&b);
    return result ? 1 : 0;
}

static int Lreader_get_num_files(lua_State* L) {
    mz_zip_archive* za = lua_touserdata(L, 1);
    lua_pushinteger(L, mz_zip_reader_get_num_files(za));
    return 1;
}

static int Lreader_get_filename(lua_State* L) {
    mz_zip_archive* za = lua_touserdata(L, 1);
    mz_uint file_index = (mz_uint)luaL_checkinteger(L, 2) - 1;
    char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
    if (!mz_zip_reader_get_filename(za, file_index,
                                    filename, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE)) {
        lua_pushnil(L);
        lua_pushfstring(L, "%d is an invalid index", file_index);
        return 2;
    }
    lua_pushstring(L, filename);
    return 1;
}

static int Lreader_is_file_a_directory(lua_State*  L) {
    mz_zip_archive* za = lua_touserdata(L, 1);
    mz_uint file_index = (mz_uint)luaL_checkinteger(L, 2) - 1;
    lua_pushboolean(L, mz_zip_reader_is_file_a_directory(za, file_index));
    return 1;
}

static int Lreader_close(lua_State* L) {
    mz_zip_archive* za = lua_touserdata(L, 1);
    lua_pushboolean(L, mz_zip_reader_end(za));
    return 1;
}

#endif

/* lminiz reader ends */

static const char* zip_get_filename(lua_State* L, int index) {
    lua_pushcfunction(L, Lreader_get_filename);
    lua_pushvalue(L, -2);
    lua_pushinteger(L, index);
    lua_call(L, 2, 1);
    return lua_tostring(L, -1);
}

static const char* zip_is_directory(lua_State* L, int index) {
    lua_pushcfunction(L, Lreader_is_file_a_directory);
    lua_pushvalue(L, -2);
    lua_pushinteger(L, index);
    lua_call(L, 2, 1);
    if (lua_toboolean(L, -1)) {
        lua_pop(L, 1);
        return NULL;
    } else {
        lua_pop(L, 1);
        return zip_get_filename(L, index);
    }
}

static int cachelar_readable(const char* filename) {
    FILE* f = fopen(filename, "r");  /* try to open file */
    if (f == NULL) return 0;  /* open failed */
    fclose(f);
    return 1;
}

static const char* cachelar_next(lua_State* L, const char* path, char separator) {
    const char* l;
    while (*path == separator) path++;  /* skip separators */
    if (*path == '\0') return NULL;  /* no more templates */
    l = strchr(path, separator);  /* find next separator */
    if (l == NULL) l = path + strlen(path);
    lua_pushlstring(L, path, l - path);  /* template */
    return l;
}

static void cachelar_add_cache(lua_State* L, const char* name, const char* mod) {
    lua_getfield(L, lua_upvalueindex(1), LAR_FIELD);  /* + 1 */
    lua_getfield(L, -1, name);  /* + 2 */
    lua_remove(L, -2);  /* - 1 */

    luaL_gsub(L, mod, PATH_SEP, ".");  /* + 2 */
    const char* s = lua_tostring(L, -1);
    lua_pushlstring(L, s, strrchr(s, '.') - s);  /* + 3 */
    s = lua_tostring(L, -1);
    lua_pop(L, 2);  /* -- 1 */

    lua_pushvalue(L, -2);  /* + 2 */
    lua_setfield(L, -2, s);  /* - 1 */
    lua_pop(L, 1);  /* - 0 */
}

static void cachelar_docacheP(lua_State* L, const char* name) {
    lua_getfield(L, lua_upvalueindex(1), LAR_FIELD);  /* + 1 */
    lua_newtable(L);  /* + 2 */
    lua_setfield(L, -2, name);  /* - 1 */
    lua_pop(L, 1);  /* - 0 */
}

static void cachelar_docacheM(lua_State* L, const char* name) {
    size_t len;
    int file_cnt, i;
    const char* mitem = NULL;
    const char* code = NULL;

    lua_pushcfunction(L, Lreader_get_num_files);  /* + 2 */
    lua_pushvalue(L, -2);  /* + 3 */
    lua_call(L, 1, 1);  /* --+ 2 */
    file_cnt = lua_tointeger(L, -1);
    lua_pop(L, 1);  /* - 1 */

    for (i = 1; i <= file_cnt; ++i ) {
        if ((mitem = zip_is_directory(L, i)) == NULL)  /* + 2 */
            continue;

        lua_pushcfunction(L, Lreader_extract);  /* + 3 */
        lua_pushvalue(L, -3);  /* + 4 */
        lua_pushinteger(L, i);  /* + 5 */
        lua_call(L, 2, 1);  /* lar:extract(i) ---+ 3 */
        code = lua_tolstring(L, -1, &len);
        luaL_loadbuffer(L, code, len, INFO_BLOCK); /* + 4 */
        cachelar_add_cache(L, name, mitem);

        lua_pop(L, 3);  /* --- 1 */
    }
    lua_pushcfunction(L, Lreader_close);
    lua_pushvalue(L, -2);
    lua_call(L, 1, 0);
}

static void cachelar_docache(lua_State* L, const char* name, const char* filename) {
    cachelar_docacheP(L, name);
    lua_pushcfunction(L, Lzip_read_file); /* + 1 */
    lua_pushstring(L, filename);  /* + 2 */
    lua_call(L, 1, 1);  /* --+ 1 */
    cachelar_docacheM(L, name);
    lua_pop(L, 1);
}

static int Lcachelar(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    lua_getfield(L, lua_upvalueindex(1), PPATH_FIELD);  /* + 1 */
    const char* path = lua_tostring(L, -1);
    lua_pop(L, 1);  /* - 0 */

    while ((path = cachelar_next(L, path, *LUA_PATH_SEP))) {  /* + 1 */
        const char* filename = luaL_gsub(L, lua_tostring(L, -1),
                                         LUA_PATH_MARK, name);  /* + 2 */
        lua_pop(L, 2);  /* -- 0 */
        if (cachelar_readable(filename)) {
            cachelar_docache(L, name, filename);
            lua_pushboolean(L, 1);
            return 1;
        }
    }

    lua_pushboolean(L, 0);
    return 1;
}

static int requiref_make_namespace_func(lua_State* L) {
    int ori = lua_gettop(L);
    lua_pushvalue(L, lua_upvalueindex(2));  /* + 1 */
    lua_setfield(L, lua_upvalueindex(1), CURRENT_FIELD);  /* - 0 */
    lua_pushvalue(L, lua_upvalueindex(3));  /* + 1 */
    lua_call(L, 0, LUA_MULTRET);  /* -[+] n */
    lua_pushnil(L);  /* + */
    lua_setfield(L, lua_upvalueindex(1), CURRENT_FIELD);  /* - */
    return lua_gettop(L) - ori;  /* return what origin function returns */
}

static void requiref_make_namespace(lua_State* L, const char* s1) {
    lua_pushvalue(L, lua_upvalueindex(1)); /* + 1 */
    lua_pushstring(L, s1); /* + 2 */
    lua_pushvalue(L, -3); /* + 3 */
    lua_pushcclosure(L, requiref_make_namespace_func, 3); /* ---+ 1 */
}

static int requiref_fetch(lua_State* L, const char* m, const char* s) {
    lua_getfield(L, lua_upvalueindex(1), LAR_FIELD);  /* + 1 */
    lua_getfield(L, -1, m);  /* + 2 */
    return lua_getfield(L, -1, s) != LUA_TNIL;  /* + 3 */
}

static const char* requiref_check_cnt(lua_State* L) {
    const char* ret = NULL;
    if (lua_getfield(L, lua_upvalueindex(1), CURRENT_FIELD) != LUA_TNIL)  /* + 1 */
        ret = lua_tostring(L, -1);
    return ret;
}

static int requiref_ensure_cached(lua_State* L) {
    int ret = 1;
    lua_getfield(L, lua_upvalueindex(1), LAR_FIELD);  /* + 1 */
    lua_pushvalue(L, -2);  /* + 2 */
    if (lua_getfield(L, -2, lua_tostring(L, -1)) == LUA_TNIL) {  /* + 3 */
        lua_pop(L, 1);  /* - 2 */
        lua_pushvalue(L, lua_upvalueindex(1));  /* + 3 */
        lua_pushcclosure(L, Lcachelar, 1);  /* -+ 3 */
        lua_pushvalue(L, -2);  /* + 4 */
        lua_call(L, 1, 1);  /* --+ 3 */
        ret = lua_toboolean(L, -1);
    }
    lua_pop(L, 3); /* --- 0 */
    return ret;
}

static int Lrequiref(lua_State* L) {
    size_t len;
    const char* r = luaL_checklstring(L, 1, &len);
    const char* s, *s1, *s2;
    if ((s = strchr(r, '.')) == NULL) s = r + len;

    if ((s2 = requiref_check_cnt(L)) != NULL) {  /* + 1 */
        if (requiref_fetch(L, s2, r))  /* + 2 */
            return 1;
        lua_pop(L, 2);  /* -- 0 */
    } else lua_pop(L, 1);  /* - 0 */

    lua_pushlstring(L, r, s - r);  /* + 1 */
    if (!requiref_ensure_cached(L)) {
        lua_pushnil(L);
        return 1;
    }

    lua_getfield(L, lua_upvalueindex(1), LAR_FIELD);  /* + 2 */
    lua_pushvalue(L, -2);  /* + 3 */
    lua_remove(L, -3);  /* - 2 */
    s1 = lua_tostring(L, -1);

    if (s == r + len) {
        if (!requiref_fetch(L, s1, INIT_FIELD))
        { lua_pushnil(L); return 1; }  /* + 3 */
        requiref_make_namespace(L, s1);  /* + 4 */
    } else {
        if (!requiref_fetch(L, s1, s + 1))
        { lua_pushnil(L); return 1; }  /* + 3 */
        requiref_make_namespace(L, s1);  /* + 4 */
    }
    return 1;
}

static int Linitexe(lua_State* L) {
    const char* exe = luaL_checkstring(L, 1);
    const char* mainb, *name;
    char buf[INFO_OFFEST + 1], *data;
    FILE* f = fopen(exe, "rb");
    int l = 0, tl, i, sum, len;

    fseek(f, -INFO_OFFEST, SEEK_END);
    fread(buf, sizeof(char), INFO_OFFEST, f);
    if (strncmp(buf, INFO_MAGIC, INFO_ID_LEN) != 0)
        luaL_error(L, "Invaild exe file");
    sscanf(buf + INFO_ID_LEN, "%d", &l);
    fseek(f, -INFO_OFFEST - l, SEEK_END);
    data = malloc(l * sizeof(char));
    fread(data, sizeof(char), l, f);
    luaL_loadbuffer(L, data, l, INFO_BLOCK);
    free(data);
    lua_call(L, 0, 2);
    fseek(f, -INFO_OFFEST - l, SEEK_END);

    mainb = lua_tostring(L, -1);
    len = luaL_len(L, -2); sum = 0;

    for (i = len; i >= 1; --i) {
        lua_geti(L, -2, i);
        lua_getfield(L, -1, "size");
        sum += lua_tointeger(L, -1);
        lua_pop(L, 2);
    }

    fseek(f, -INFO_OFFEST - l - sum, SEEK_END);
    for (i = 1; i <= len; ++i) {
        lua_geti(L, -2, i);
        lua_getfield(L, -1, "id");
        lua_getfield(L, -2, "size");
        tl = lua_tointeger(L, -1);
        data = malloc(tl * sizeof(char));
        fread(data, sizeof(char), tl, f);
        name = lua_tostring(L, -2);
        cachelar_docacheP(L, name);
        lua_pushcfunction(L, Lzip_read_mem);
        lua_pushlightuserdata(L, data);
        lua_pushinteger(L, tl);
        lua_call(L, 2, 1);
        cachelar_docacheM(L, name);
        lua_pop(L, 4);
        free(data);
    }
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushcclosure(L, Lrequiref, 1);
    lua_pushstring(L, mainb);
    lua_call(L, 1, 1);
    lua_call(L, 0, 0);
    lua_pushboolean(L, 1);
    return 1;
}

__attribute ((visibility("default"))) int luaopen_larpr(lua_State* L) {
    luaL_Reg l[] = {
#define ENTRY(name) { #name, L##name }
        ENTRY(cachelar),
        ENTRY(requiref),
        ENTRY(initexe),
#undef  ENTRY
        { NULL, NULL }
    };

    /* initialize */
    lua_settop(L, 0);
    lua_newtable(L);  /* + 1 */
    lua_pushliteral(L, LUA_PPATH_DEFAULT);  /* + 2 */
    lua_setfield(L, -2, PPATH_FIELD);  /* - 1 */
    lua_newtable(L);  /* + 2 */
    lua_setfield(L, -2, LAR_FIELD); /* - 1 */
    lua_pushliteral(L, LARPR_VERSION);  /* + 2 */
    lua_setfield(L, -2, VERSION_FIELD); /* - 1 */

    /* set global upvalue */
    lua_pushvalue(L, -1);  /* + 2 */
    luaL_setfuncs(L, l, 1);  /* - 1 */

    /* set package seachers */
    luaL_requiref(L, "package", luaopen_package, 1);  /* + 2 */
    lua_getfield(L, -1, LUA_SEARCHERS);  /* + 3 */
    lua_getfield(L, -3, "requiref");  /* + 4 */
    lua_seti(L, -2, luaL_len(L, -2) + 1);  /* - 3 */
    lua_pop(L, 2);  /* -- 1 */

    return 1;
}

__attribute ((visibility("default"))) int larpr_init(lua_State* L, const char* exe) {
    luaL_requiref(L, LARPR_NAME, luaopen_larpr, 1);
    if (exe != NULL) {
        lua_pushcclosure(L, Linitexe, 1);
        lua_pushstring(L, exe);
        lua_call(L, 1, 0);
    } else lua_pop(L, 1);
    return 1;
}

__attribute ((visibility("default"))) int larpr_requiref(lua_State* L, const char* module) {
    luaL_requiref(L, LARPR_NAME, luaopen_larpr, 1);
    lua_pushcclosure(L, Lrequiref, 1);
    lua_pushstring(L, module);
    lua_call(L, 1, 1);
    return 1;
}

__attribute ((visibility("default"))) int larpr_setppath(lua_State* L, const char* path) {
    luaL_requiref(L, LARPR_NAME, luaopen_larpr, 1);
    lua_pushstring(L, path);
    lua_setfield(L, -2, PPATH_FIELD);
    lua_pop(L, 1);
    return 1;
}
