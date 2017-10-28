
-- New meta table for class
Document = {
	__metatable = "[Protected metatable]", -- Protect metatable from changing
	__bson = 3  --bson type document
}

-- Use metatable as index
Document.__index = Document

function Document:__newindex(key, value)
	rawset(self, key, value)
	rawset(self.__order, self.__nextIndex, key)
	--rawset(self.__keys, key, self.__nextIndex)
	self.__nextIndex = self.__nextIndex + 1
end

function Document:__tostring()
	return "[BSON document]"
end

--[[
function Document:delete(key) 
	if self[key] then
		--Remove value from main table
		rawset(self, key, nil)
		--Remove key from order table
		rawset(self.__order, rawget(self.__keys, key), nil)
	end
end
]]

function Document:delete(key) 
	if self[key] then
		--Remove value from main table
		rawset(self, key, nil)
		--Remove key from order table
		for i=1, #self.__order do
			if self.__order[i] == key then
				rawset(self.__order, i, nil)
				break
			end
		end
	end
end

function Document:pairs()
	local k = nil
	return function(a, b,c) 
		local v;
		k, v = next(self.__order, k)
		if v then
			return v, self[v]
		end	
	end
end

-- Constructs a new Document
function Document:new(t) 
	--local newTable = setmetatable({ __order = {}, __keys = {}, __nextIndex = 1 }, Document)
	local newTable = setmetatable({ __order = {}, __nextIndex = 1 }, Document)
	if (t) then
		for i = 1, #t, 2 do
			newTable[t[i]] = t[i+1]
		end
	end
	return newTable
end

setmetatable(Document, {
	__call = function(self, t)
		return Document:new(t)
	end,
	__tostring = function()
		return "[Document constructor]"
	end
})