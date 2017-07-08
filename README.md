# larpr - Lua ARchive support
[![Build Status](https://travis-ci.org/avaicode/larpr.svg?branch=master)](https://travis-ci.org/avaicode/larpr)
[![Coverage Status](https://coveralls.io/repos/github/avaicode/larpr/badge.svg?branch=master)](https://coveralls.io/github/avaicode/larpr?branch=master)

larpr is a jar-like archive support,which allows you to pack multi lua source or bytecode into one zipped file.

## Building
larpr use CMake to build.It is compatible with lua 5.x and luajit 2.x.

You can use `-DEMINIZ=ON` in CMake to switch larpr to use external [miniz](https://github.com/avaicode/lua-miniz).

## Usage

```lua
require("larpr")
```
After requiring, lua's package loader will be updated to find lars.For example,to find foo.luac in test.lar,type following code.
```lua
require("test.foo")
```

## Functions

- `larpr.cachelar([name(string)]) -> boolean`
> Cache lar that matches `larpr.ppath` which is similar to lua's `package.path`

- `larpr.requiref([name(string)]) -> function`
> Return a loaded function by name.If the lar which contains the module is not cached, it will cache it first.
## License

MIT License
