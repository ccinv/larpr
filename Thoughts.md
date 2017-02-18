# Package requiring in the same lar file
>test.lar: { "./foo.lua", "./big.lua", "./major/bigfool.lua" }

``` lua
-- foo.lua
require "big"  -- notice:not "test.big"
require "major.bigfool"  -- support too
...
```

``` lua
-- main.lua
require "test.foo"  -- "foo" is not supported
require "test.big"
```

## Implementation
Set a item of `_ENV` before caching the lar, then `lar.require` inside the lar will know it's in the archive. 
# init.lua
It's just like init.go. When lar package is cached,init.go is called to set up the initial env.And any methods else can get the env created by init.go

``` lua
-- init.lua

return function(env)
    env.PI = 3.14
end
```
``` lua
-- foo.lua(another lua file in the lar)

return function()
    -- __getenv is provided by compiler, but maybe we can do it in a better way
    print(__getenv().PI)
end
```