--luacheck: no global
--luacheck: no unused args

require "miniz"
package.loaded["larpr"] = nil
local info = require("path.info")
if info.platform == "windows" then package.cpath = package.cpath .. ";./builds/?.dll" end
if info.platform == "linux" then package.cpath = package.cpath .. ";./builds/?.so" end
local M = require "larpr"
M.ppath = M.ppath .. ";./spec/?.lar"

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
end)
