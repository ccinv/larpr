-- The MIT License (MIT)
-- Copyright (c) 2016 Yanghe Sun

-- freeze lar files to build a self-running program
-- Required module:
-- lua_cliargs, moonscript

return (arg) ->
    fs    = require("path.fs")
    cli   = require("cliargs")
    path  = require("path")
    moon  = require("moon")
    load  = loadstring or load

    import split, splitext from path

    COPYRIGHT = "Copyright (C) 1994-2017 Yanghe Sun"
    VERSION = "freeze packager 0.1b"

    print_version = ->
        print(VERSION .. "\n" .. COPYRIGHT)
        os.exit(0)

    cli\set_name("freeze.moon")
    cli\argument("ORIG", "orig executeable file")
    cli\argument("MAIN", "main lar file to execute")
    cli\argument("OUTPUT", "path to the output executeable file")
    cli\splat("INPUTS", "lar files to package", "", 100)
    cli\flag("-v, --version", "prints the program's version and exits", print_version)
    cli\flag("--verbose", "the script output will be very verbose")
    args, err = cli\parse(arg)
    cli\cleanup!
    vprint = (...) -> print(...) if args["verbose"]

    if err then
        print(err)
        os.exit(0)

    get_sizegroup = (ori) ->
        ret = {}
        for u, v in ipairs(ori)
            ret[u] = fs.fsize(v)
        return ret

    dumps = (tbl, main) ->
        tbcode = "{"
        for _, v in ipairs(tbl)
            subcode = "{"
            subcode ..= string.format("size=%d,", v.size)
            subcode ..= string.format("id=\"%s\"", v.id)
            subcode ..= "},"
            tbcode ..= subcode
        tbcode ..= "}"
        funcode = string.format("return %s, \"%s\"", tbcode, main)
        return string.dump(load(funcode), true)

    create_meta = (group, main)->
        size = get_sizegroup(group)
        ret  = {}
        for i= 1, #group
            ret[i] = { size: size[i],
                       id: splitext(select(2, split(group[i]))) }
        meta = dumps(ret, main)
        info = string.format("<larpr%010d>", #meta)
        return meta, info

    main = () ->
        vprint(VERSION)
        meta, info = create_meta(args.INPUTS, args.MAIN)
        f = io.open(args.OUTPUT, "wb")
        f1 = io.open(args.ORIG, "rb")
        f\write(f1\read("*a"))

        for v in *args.INPUTS
            f1 = io.open(v, "rb")
            f\write(f1\read("*a"))
            f1\close()
            vprint("- Sucessfully freezed " .. v)

        f\write(meta)
        f\write(info)
        f\close()

    main!
