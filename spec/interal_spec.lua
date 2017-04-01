require "miniz"
package.loaded["larpr"] = nil
local M = require "larpr"

--luacheck: no global
--luacheck: no unused args

describe("interal loading Testing", function()
    it("checks interal loading", function()
        assert.are.equal(M.initexe("container"), true)
    end)
    it("checks interal require", function()
        assert.are.equal(require("testcases.foo"), 3.14 + 2017 + 1)
    end)
end)
