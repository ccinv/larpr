local ct
if require("path.info").platform == "windows" then
    ct = "../build/container.exe"
else
    ct = "../build/container"
end

local ot
if require("path.info").platform == "windows" then
    ot = "larpr1.exe"
else
    ot = "larpr"
end

return {
    main = "larcli",
    output = ot,
    container = ct,
    modules = {
        { srcdir = ".", name = "larcli", main = "larcli" }
    },
    ext = {
        { name = "lua_cliargs", alais = "cliargs", main = "cliargs" },
        { name = "moonscript", alais = "moonscript", main = "moonscript" }
    }
}