/*
** The MIT License (MIT)
** Copyright (c) <2016> <avaicode>
*/

#define LARPR_VERSION "larpr 1.0rc"

#define INIT_FIELD    "_init"
#define LUA_PATH_SEP  ";"
#define LUA_PATH_MARK "?"
#define PATH_SEP      "/"

#define LARPR_NAME    "larpr"
#define LAR_FIELD     "_LARS"
#define CURRENT_FIELD "_CNT"
#define PPATH_FIELD   "ppath"
#define VERSION_FIELD "VERSION"
#ifdef _WIN32
    #define LUA_PPATH_DEFAULT   ".\\?.lar"
#else
    #define LUA_PPATH_DEFAULT   "./?.lar"
#endif

#define INFO_OFFEST 17
#define INFO_ID_LEN 6
#define INFO_MAGIC "<larpr"
#define INFO_BLOCK "interal"

#ifdef USE_EXTERN_MINIZ
    #define EXTERN_MINIZ_NAME "miniz"
#endif
