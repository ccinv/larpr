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
    args, err = cli\parse(arg)
    cli\cleanup!
    verbose = ""
    verbose = "--verbose" if args["verbose"]
    vprint = (...) -> print(...) if args["verbose"]

    if err then
        print(err)
        os.exit(0)

    main = ()->
        vprint(VERSION)
        script = dofile(args.PATH)
        main      = script.main
        srcdir    = path.abs(script.srcdir)
        output    = path.abs(script.output)
        modules   = script.modules
        container = path.abs(script.container)
        cwd = fs.getcwd!
        wd = fs.tmpdir!
        assert(fs.chdir(wd))
        lars = {}
        for v in *modules
            fetch({ verbose, v.name, v.main, v.alais })
            table.insert(lars, v.alais .. ".lar")
        lar({ "-s", "-b", verbose, srcdir, main, "main.lar" })
        table.insert(lars, "main.lar")
        freeze({ verbose, container, "main", output, table.unpack(lars) })

    main!
