# larpr - Lua Archive Support

**Not in maintain anymore**

`larpr` is a JAR-like archive loader for Lua that packs multiple Lua source (`.lua`) or bytecode (`.luac`) files into a single ZIP archive (`.lar`) and integrates directly with Lua's `require` mechanism.

## Building

`larpr` uses CMake and supports Lua 5.1, 5.2, 5.3 and LuaJIT 2.x.

```bash
git submodule update --init --recursive
mkdir build && cd build
cmake ..
make
```

### CMake Options

- `-DBUILD_SHARED_LIBS=ON` (default `ON`): Build shared library (`larpr.so` / `larpr.dll`).
- `-DBUILD_STATIC_LIBS=OFF` (default `OFF`): Build static library.
- `-DEMINIZ=ON` (default `OFF`): Use external [lua-miniz](https://github.com/starwing/lua-miniz) instead of the bundled `miniz`.

## Archive Structure (`.lar`)

A `.lar` file is a standard ZIP archive containing `.lua` or `.luac` files:

```text
test.lar
├── _init.lua        -- Entry point when calling require("test")
├── foo.lua          -- Loaded via require("test.foo") or require("foo") inside test.lar
└── deps/
    └── lucky.lua    -- Loaded via require("test.deps.lucky") or require("deps.lucky") inside test.lar
```

- **Package Entry (`_init`)**: Requiring the archive name directly (`require("test")`) executes `_init.lua` (or `_init.luac`) inside `test.lar`.
- **Internal Namespace**: While a module inside `test.lar` is executing, relative `require` calls (such as `require("foo")` or `require("deps.lucky")`) automatically resolve within `test.lar` first.

## Lua API

```lua
local larpr = require("larpr")

-- Configure search path for .lar archives (default: "./?.lar" or ".\?.lar" on Windows)
larpr.ppath = larpr.ppath .. ";./spec/?.lar"

-- Require a module inside test.lar
local foo = require("test.foo")

-- Require a nested module inside test.lar
local lucky = require("test.deps.lucky")

-- Require the archive's _init entry module
local init = require("test")
```

### Fields & Functions

- `larpr.ppath` (`string`)
  > Search path template for `.lar` files, formatted like Lua's `package.path` (e.g., `"./?.lar;./lib/?.lar"`).

- `larpr.VERSION` (`string`)
  > Version string (`"larpr 1.0rc"`).

- `larpr.cachelar(name) -> boolean`
  > Searches `larpr.ppath` for `<name>.lar`, loads and compiles all chunks in the archive into memory, and returns `true` on success (or `false` if not found / invalid).

- `larpr.requiref(name) -> function | nil`
  > Package searcher registered in `package.searchers` / `package.loaders`. Returns the loader function for `name` (caching the archive first if needed), or `nil` if not found.

## C API

Declared in [`src/larpr.h`](src/larpr.h):

```c
int luaopen_larpr(lua_State* L);
int larpr_requiref(lua_State* L, const char* module);
int larpr_setppath(lua_State* L, const char* path);
```

## License

MIT License
