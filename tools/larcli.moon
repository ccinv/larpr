#!/usr/local/bin/moon
-- The MIT License (MIT)
-- Copyright (c) 2016 Yanghe Sun

switch arg[1]
    when "lar"
        table.remove(arg, 1)
        require("lar")(arg)
    when "fetch"
        table.remove(arg, 1)
        require("fetch")(arg)
    else
        words = { "lar", "fetch" }
        io.write("Following commands are supported:\n")
        for v in *words
            io.write(v,' ')
        io.write('\n')

