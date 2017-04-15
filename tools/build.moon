-- The MIT License (MIT)
-- Copyright (c) 2016 Yanghe Sun

-- Makefile for lua!
-- Required module:
-- lua_cliargs, moonscript

return (arg) ->
    lar    = require("lar")
    fetch  = require("fetch")
    freeze = require("freeze")

    fs    = require("path.fs")
    cli   = require("cliargs")
    path  = require("path")
    unpack = unpack or table.unpack

    COPYRIGHT = "Copyright (C) 1994-2017 Yanghe Sun"
    VERSION = "lua proj builder 0.1b"

    print_version = ->
        print(VERSION .. "\n" .. COPYRIGHT)
        os.exit(0)

    cli\set_name("fetch.moon")
    cli\argument("PATH", "path of building script")
    cli\flag("-v, --version", "prints the program's version and exits", print_version)
    cli\flag("--verbose", "the script output will be very verbose")
    table.remove(arg, 1) if arg[1] == ""
    args, err = cli\parse(arg)
    cli\cleanup!
    if err then
        print(err)
        os.exit(0)

    verbose = ""
    verbose = "--verbose" if args["verbose"]
    vprint = (...) -> print(...) if args["verbose"]

    main = ()->
        vprint(VERSION)
        script = dofile(args.PATH)
        main      = script.main
        ext       = script.ext
        cache     = path.abs(path.splitext(args.PATH) .. ".cache")
        modules   = script.modules
        output    = path.abs(script.output)
        container = path.abs(script.container)
        for v in *modules
            v.srcdir = path.abs(v.srcdir)

        cwd = fs.getcwd!
        wd = fs.tmpdir!

        assert(fs.chdir(wd))
        lars = {}
        for v in *modules
            lar({ verbose, "-s", "-b", v.srcdir, v.main, v.name .. ".lar" })
            table.insert(lars, v.name .. ".lar")

        if fs.exists(cache)
            for v in *dofile(cache)
                table.insert(lars, v)
        else
            f = io.open(cache, "w")
            f\write("return {")
            for v in *ext
                fetch({ verbose, v.name, v.main, v.alais })
                table.insert(lars, v.alais .. ".lar")
                f\write('[[' .. path.abs(v.alais .. ".lar") .. ']],')
            f\write("}")
            f\close!

        freeze({ verbose, container, main, output, table.unpack(lars) })

    main!
