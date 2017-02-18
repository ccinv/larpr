require "miniz"
local M = require "larpr"

--luacheck: no global
--luacheck: no unused args

describe("larpr Testing", function()
    it("checks cachelar", function()
        assert.is_true(M.cachelar("testcases"))
    end)
    it("checks simple require", function()
        assert.are.equal(require("testcases.big").PI, 3.14)
    end)
    it("checks deeep require", function()
        assert.are.equal(require("testcases.foo"), 3.14 + 2017 + 1)
    end)
    it("checks folder deep require", function()
        assert.are.equal(require("testcases.deps.lucky"), "lucky~!")
    end)
end)
