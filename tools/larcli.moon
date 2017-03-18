-- The MIT License (MIT)
-- Copyright (c) 2016 Yanghe Sun

switch arg[1]
    when "lar"
        table.remove(arg, 1)
        require("lar")(arg)
    when "freeze"
        table.remove(arg, 1)
        require("freeze")(arg)
    when "fetch"
        table.remove(arg, 1)
        require("fetch")(arg)
    when "build"
        table.remove(arg, 1)
        require("build")(arg)
    else
        words = { "lar", "freeze", "fetch", "build" }
        io.write("Following commands are supported:\n")
        for v in *words
            io.write(v,' ')
