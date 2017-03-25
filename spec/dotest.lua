-- The MIT License (MIT)
-- Copyright (c) 2016 Yanghe Sun

local info = require("path.info")

if info.platform == "windows" then
    local cmd = ""
    cmd = cmd .. [[cmd /c moon .\tools\larcli.moon lar -b -s .\spec\testcases init testcases.lar&&]]
    cmd = cmd .. [[copy nul raw > nul&&]]
    cmd = cmd .. [[cmd /c moon .\tools\larcli.moon freeze raw testcases container testcases.lar&&]]
    cmd = cmd .. [[cmd /c busted]]
    os.execute(cmd)
end

if info.platform == "linux" then
    local cmd = ""
    cmd = cmd .. [[./tools/larpr lar -b -s ./spec/testcases init testcases.lar&&]]
    cmd = cmd .. [[touch raw&&]]
    cmd = cmd .. [[./tools/larpr freeze raw testcases container testcases.lar&&]]
    cmd = cmd .. [[busted]]
    os.execute(cmd)
end
