
function find()
	local t1 = time();
	db.test:find():forEach(function(doc) end);
	print("ForEach\t\t", (time() - t1), "ms");
end

--db.test:remove() printjson(db.test:createTestData(2000000))
--db.test:remove() printjson(db.test:createTestData(1))
--printjson(db.test:findOne(), true);

--[[
local size = 1000000
local docs = {};

local t1 = time();
for i=1, size do
docs[i] = Document({
	"a",-5,
	"b", 7,
	"c", -2.5,
	"d", 7.9,
	"e", true,
	"f", false,
	"g", "hej",
	"h", null,
	"i", {1,2,3, "hello", null, true, false, -1, 1, 5.5, 2.74},
	"j", {a = 2},
	"k", {}
})
end
print("create", (time() - t1), "ms");

local t1 = time();
for i=1, size do
docs[i]:delete("k")
end
print("delete", (time() - t1), "ms");
]]

--find()
--cTest()

print(9223372036854775807)




