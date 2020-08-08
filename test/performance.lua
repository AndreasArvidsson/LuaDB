local coll = db:getCollection("test")
local size = 100000

function run(name, callback)
	local t0 = time()
	callback()
	local t1 = time()
	print(name, t1 - t0, "ms")
end

-- Clean state
coll:remove({})

print("\nSize", "\t", size, "\n")

run("Insert {}\t", function()
	for i=1, size do
		coll:insert({})
	end
end)

run("Insert table\t", function()
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
	for i=1, size do
		coll:insert(table)
	end
end)

run("Insert table inline", function()
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
end)

run("Insert document\t", function()
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
	for i=1, size do
		coll:insert(doc);
	end
end)

run("Insert document inline", function()
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
end)

run("insertMany doc\t", function()
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
	local docs = {};
	for i=1, size do
		docs[i] = doc;
	end
	coll:insertMany(docs);
end)

run("count\t\t", function()
	coll:count({a = -5});
end)

run("cursor:next\t", function()
	local cursor = coll:find({});
	while(cursor:next()) do 
	end
end)

run("forEach\t\t", function()
	coll:find():forEach(function(doc) 
	end)
end)

run("iterator\t", function()
	for result in coll:find():iterator() do 
	end
end)

run("toArray\t\t", function()
	coll:find():toArray()
end)

run("remove\t\t", function()
	coll:remove({});
end)

run("Empty loop\t", function()
	for i=1, size * 1000 do
	end
end)

run("Function call\t", function()
	function func() end
	for i=1, size * 1000 do
		func();
	end
end)