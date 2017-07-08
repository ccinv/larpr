-- The MIT License (MIT)
-- Copyright (c) 2016 Yanghe Sun

local info = require("path.info")
if info.platform == "windows" then
    local cmd = ""
    cmd = cmd .. [[cd tools&&cmd /c moon larcli.moon lar -b -s ..\spec\testcases init ..\testcases.lar&&cd ..&&]]
    cmd = cmd .. [[cmd /c busted]]
    os.execute(cmd)
    os.execute([[del testcases.lar]])
end

if info.platform == "linux" then
    local cmd = ""
    cmd = cmd .. [[cd tools&&./larcli.moon lar -b -s ../spec/testcases init ../testcases.lar&&cd ..&&]]
    cmd = cmd .. [[busted]]
    os.execute(cmd)
    os.execute([[rm testcases.lar]])
end
