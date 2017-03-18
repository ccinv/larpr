-- The MIT License (MIT)
-- Copyright (c) 2016 Yanghe Sun

-- lar Generator and lua/moon builder
-- Required module:
-- lua-miniz, lua_cliargs, lpath, moonscript

return (arg) ->
    fs    = require("path.fs")
    cli   = require("cliargs")
    miniz = require("miniz")
    path  = require("path")
    moons = require("moonscript")
    import join from path

    COPYRIGHT = "Copyright (C) 1994-2017 Yanghe Sun"
    VERSION = "lar compiler 0.1.1b"

    print_version = ->
        print(VERSION .. "\n" .. COPYRIGHT)
        os.exit(0)

    cli\set_name("lar.moon")
    cli\argument("FOLDER", "the source files' folder")
    cli\argument("MAIN", "main script of the lar file")
    cli\argument("OUTPUT", "path to the output lar file")
    cli\flag("-b, --byte", "build source files into byte code")
    cli\flag("-s, --strip", "strip when building byte code")
    cli\flag("-v, --version", "prints the program's version and exits", print_version)
    cli\flag("--verbose", "the script output will be very verbose")
    args, err = cli\parse(arg)
    cli\cleanup!

    if err then
        print(err)
        os.exit(0)

    checkext = (src) ->
        ext = select(2, path.splitext(src))
        invild = { ".moonc", ".moon", ".lua", ".luac" }
        for v in *invild
            return true if ext == v
        return false

    buildmoon = (src, dst) ->
        f = io.open(dst, "w")
        v, _ = moons.to_lua(io.open(src)\read("*a"))
        f\write(v)
        f\close()
    buildlua =  (src, dst) ->
        assert(fs.copy(src, dst))

    dump = (src, dst, strip) ->
        f = io.open(dst, "wb")
        f\write(string.dump(loadfile(src), strip))
        f\close()

    makeinit = (field) -> string.format([[return require("%s")]], field)

    build = (src, dst) ->
        getext = (p) -> select(2, path.splitext(p))
        nt = os.tmpname()

        switch getext(src)
            when ".moon"
                buildmoon(src, nt)
            when ".lua"
                buildlua(src, nt)
            when ".luac"
                assert(fs.copy(src, dst))
                os.remove(nt)
                return true

        if not args['b'] then
            assert(fs.copy(nt, dst))
            os.remove(nt)
            return false
        dump(nt, dst, args['s'])
        os.remove(nt)
        return true

    add = (o, cnt, base) ->
        for v, t in fs.dir(cnt) do
            if t == "dir" then
                if base == "" then
                    add(o, join(cnt, v), v)
                else
                    add(o, join(cnt, v), base .. "/" .. v)
            if t == "file" and checkext(v) then
                t = os.tmpname()
                if build(join(cnt, v), t) then
                    if args['verbose'] then
                        print("Built " .. join(cnt, v))
                    v = path.splitext(v) .. ".luac"
                else
                    if args['verbose'] then
                        print("Passing " .. join(cnt, v))
                    v = path.splitext(v) .. ".lua"
                if base == "" then
                    assert(o\add_file(v, t))
                else
                    assert(o\add_file(base .. "/" .. v, t))
                os.remove(t)

    main = () ->
        o = miniz.zip_write_file(args.OUTPUT)
        add(o, args.FOLDER, "")
        o\add_string("_init.lua", makeinit(args.MAIN))
        o\finalize()
        o\close()

    main!
