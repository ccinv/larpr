-- The MIT License (MIT)
-- Copyright (c) 2016 Yanghe Sun

-- Fetch packages from luarocks and lar it
-- Required module:
-- lua_cliargs, moonscript, luarocks, path

return (arg) ->
    lar   = require("lar")

    fs    = require("path.fs")
    cli   = require("cliargs")
    path  = require("path")

    COPYRIGHT = "Copyright (C) 1994-2017 Yanghe Sun"
    VERSION = "fetching packager 0.1b"

    print_version = ->
        print(VERSION .. "\n" .. COPYRIGHT)
        os.exit(0)

    cli\set_name("fetch.moon")
    cli\argument("NAME", "name of package to fetch")
    cli\argument("MAIN", "name of the main script")
    cli\splat("ALAIS", "name of output file", "[NONE]", 1)
    cli\flag("-v, --version", "prints the program's version and exits", print_version)
    cli\flag("--verbose", "the script output will be very verbose")
    table.remove(arg, 1) if arg[1] == ""
    args, err = cli\parse(arg)
    cli\cleanup!
    if err then
        print(err)
        os.exit(0)

    args.ALAIS =  args.MAIN  if args.ALAIS == "[NONE]"
    verbose = ""
    verbose = "--verbose" if args["verbose"]
    vprint = (...) -> print(...) if args["verbose"]

    rrpath = (td, name) ->
        name = string.gsub(name, "%.", "/")
        return path.join(td, name) .. ".lua"

    rockspec_move = (rkfile, cwd) ->
        wd = fs.tmpdir!
        env = {}
        loadfile(rkfile, "bt", env)()
        modules = env.build.modules
        for k, v in pairs(modules)
            t = rrpath(wd, k)
            tt = select(1, path.split(t))
            assert(fs.makedirs(tt))
            assert(fs.copy(v, rrpath(wd, k)))
        tar = args.ALAIS .. ".lar"
        lar({ "-s", "-b", verbose, wd, args.MAIN, tar })
        assert(fs.copy(tar, path.join(cwd, args.ALAIS .. ".lar")))
        fs.removedirs(wd)

    main = () ->
        vprint(VERSION)
        vprint("- Fetching " .. args.NAME .. " from luarocks")
        cwd = fs.getcwd!
        wd = fs.tmpdir!
        assert(fs.chdir(wd))
        cmd = string.format("luarocks unpack %s > tmp.txt", args.NAME)
        os.execute(cmd)
        f = io.open("tmp.txt")
        rpath = f\read!
        rpath = f\read!
        rpath = f\read!  -- the third line
        f\close!
        fs.chdir(rpath)
        rkfile = ""
        for v, t in fs.dir(".")
            continue if t != "file"
            continue if select(2, path.splitext(v)) != ".rockspec"
            rkfile = v
            break
        vprint("- Selected rockspec " .. rkfile)
        rockspec_move(rkfile, cwd)
        vprint("- Sucessfully fetched " .. args.MAIN)
        assert(fs.chdir(cwd))
        fs.removedirs(wd)

    main!
