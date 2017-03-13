-- The MIT License (MIT)
-- Copyright (c) 2016 Yanghe Sun

local info = require("path.info")

if info.platform == "windows" then
    local cmd = ""
    cmd = cmd .. [[mkdir tmp&&]]
    cmd = cmd .. [[copy lar.moon tmp > nul&&]]
    cmd = cmd .. [[cmd /c moon lar.moon -b -s tmp lar.lar&&]]
    cmd = cmd ..
    string.format([[cmd /c moon freeze.moon %s lar lar.exe lar.lar&&]], arg[1])
    cmd = cmd .. [[del /S /Q tmp > nul&&copy freeze.moon tmp > nul&&]]
    cmd = cmd .. [[cmd /c moon lar.moon -b -s tmp freeze.lar&&]]
    cmd = cmd ..
    string.format([[cmd /c moon freeze.moon %s freeze freeze.exe freeze.lar&&]], arg[1])
    cmd = cmd .. [[rd /S /Q tmp > nul&&del freeze.lar lar.lar]]
    os.execute(cmd)
end
