
print("Test CRUD operations")

-- Clean state
db.test:remove({usedByTest = true})

-- INSERT
local wr = db:getCollection("test"):insert({usedByTest = true})
assertEquals(1, wr.nInserted, "Test insert")

-- UPSERT
local wr = db.test:update({_id = "nonExistingId"}, {usedByTest = true}, {upsert = true})
assertEquals(1, wr.nUpserted, "Test upsert")

-- UPDATE
local wr = db.test:update({usedByTest = true}, {usedByTest = true, updated = true})
assertEquals(1, wr.nModified, "Test update")

-- COUNT
assertEquals(2, db.test:count({usedByTest = true}), "Test count")

-- DISTINCT
local res = db.test:distinct("_id", {usedByTest = true})
assertEquals(2, #res, "Test distinct")

-- FIND NEXT
local cursor = db.test:find({usedByTest = true})
local count = 0;
local doc = cursor:next()
assert(doc ~= nil, "Cursor.next returned nil")
while (doc) do
	doc = cursor:next()
	count = count + 1
end
assertEquals(2, count, "Test find next")

-- FIND FOR EACH
local count = 0;
db.test:find({usedByTest = true}):forEach(function(doc)
	assert(doc ~= nil, "Cursor.forEach returned nil")
	count = count + 1
end)
assertEquals(2, count, "Test find forEach")

-- FIND ITERATOR
local count = 0;
for doc in db.test:find({usedByTest = true}):iterator() do
	assert(doc ~= nil, "Cursor.iterator returned nil")
	count = count + 1
end
assertEquals(2, count, "Test find iterator")

-- FIND TO ARRAY
local arr = db.test:find({usedByTest = true}):toArray()
assertEquals(2, #arr, "Test find toArray")

-- FIND TO ARRAY / LIMIT
local arr = db.test:find({usedByTest = true}):limit(1):toArray()
assertEquals(1, #arr, "Test find toArray / limit")

-- FIND ONE
local doc = db.test:findOne({usedByTest = true, updated = true})
assert(doc ~= nil, "findOne is nil")

-- REMOVE
local wr = db.test:remove({usedByTest = true})
assertEquals(2, wr.nRemoved, "Test remove")