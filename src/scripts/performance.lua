
local coll = db:getCollection("test")
local t1
local size = 100000

local table = {
	a = -5,
	b = 7,
	c = -2.5,
	d = 7.9,
	e = true,
	f = false,
	g = "hej",
	h = null,
	i = {1,2,3, "hello", null, true, false, -1, 1, 5.5, 2.74},
	j = {a = 2},
	k = {}
}

local doc = Document({
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

-- Clean state
coll:remove({})

print("\nSize\t\t", size, "\n")

local t1 = time();
for i=1, size do
	coll:insert({})
end
print("Insert {}\t", (time() - t1), "ms")

local t1 = time();
for i=1, size do
	coll:insert(table)
end
print("Insert table\t", (time() - t1), "ms")

local t1 = time();
for i=1, size do
	coll:insert({
	a = -5,
	b = 7,
	c = -2.5,
	d = 7.9,
	e = true,
	f = false,
	g = "hej",
	h = null,
	i = {1,2,3, "hello", null, true, false, -1, 1, 5.5, 2.74},
	j = {a = 2},
	k = {}
})
end
print("Insert table inline", (time() - t1), "ms");

local t1 = time();
for i=1, size do
	coll:insert(doc);
end
print("Insert document\t", (time() - t1), "ms");

local t1 = time();
for i=1, size do
	coll:insert(Document({
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
}));
end
print("Insert doc inline", (time() - t1), "ms");

local t1 = time();
local docs = {};
for i=1, size do
	docs[i] = doc;
end
coll:insertMany(docs);
print("InsertMany doc\t", (time() - t1), "ms");

local t1 = time();
coll:count({a = -5});
print("Count\t\t", (time() - t1), "ms");

local t1 = time();
local cursor = coll:find({});
while(cursor:next()) do end
print("Next\t\t", (time() - t1), "ms");

local t1 = time();
coll:find():forEach(function(doc) 
end);
print("ForEach\t\t", (time() - t1), "ms");

local t1 = time();
for result in coll:find():iterator() do
end
print("Iterator\t", (time() - t1), "ms");

local t1 = time();
coll:find():toArray()
print("toArray\t\t", (time() - t1), "ms");

local t1 = time();
coll:remove({});
print("Remove\t\t", (time() - t1), "ms");

local t1 = time();
for i=1, size * 1000 do
end
print("Empty loop\t", (time() - t1), "ms");

function func() end
local t1 = time();
for i=1, size * 1000 do
	func();
end
print("Function call\t", (time() - t1), "ms");