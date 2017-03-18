local ct
if require("path.info").platform == "windows" then
    ct = "../build/container.exe"
else
    ct = "../build/container"
end

local ot
if require("path.info").platform == "windows" then
    ot = "larpr.exe"
else
    ot = "larpr"
end

return {
    main = "larcli",
    srcdir = ".",
    output = ot,
    container = ct,
    modules = {
        { name = "lua_cliargs", alais = "cliargs", main = "cliargs" },
        { name = "moonscript", alais = "moonscript", main = "moonscript" }
    }
}