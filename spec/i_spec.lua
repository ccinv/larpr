require "miniz"
local M = require "larpr"

--luacheck: no global
--luacheck: no unused args

os.execute("moon ./tools/lar.moon -b -s ./spec/testcases testcases.lar")
os.execute("moon ./tools/freeze.moon container testcases testing testcases.lar")
os.execute("./testing > check.txt")

describe("larpr Testing", function()
    it("checks simple require", function()
        assert.are.equal(require("testcases.big").PI, 3.14)
    end)
    it("checks deeep require", function()
        assert.are.equal(require("testcases.foo"), 3.14 + 2017 + 1)
    end)
    it("checks folder deep require", function()
        assert.are.equal(require("testcases.deps.lucky"), "lucky~!")
    end)
    it("checks initializing require", function()
        assert.are.equal(require("testcases"), 3.14)
    end)
    it("checks interal loading", function()
        assert.are.equal(io.open("check.txt"):read("*a"), "3.14")
    end)
end)
