return {
    main = "larcli",
    output = "larpr",
    container = "../builds/container",
    modules = {
        { srcdir = ".", name = "larcli", main = "larcli" }
    },
    ext = {
        { name = "lua_cliargs", alais = "cliargs", main = "cliargs" },
        { name = "moonscript", alais = "moonscript", main = "moonscript" }
    }
}