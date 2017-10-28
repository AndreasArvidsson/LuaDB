
NumberInt2 = {
	__metatable = "Protected",
	__bson = 16  --bson type int32
}

NumberInt2.__index = NumberInt2

function NumberInt2:__tostring()
	return "Int32 instance"
end

function NumberInt2:new(v) 
	return setmetatable({v or 0}, NumberInt2)
end

setmetatable(NumberInt2, {
	__call = function(self, v)
		return NumberInt2:new(v)
	end,
	__tostring = function()
		return "NumberInt2 constructor"
	end
})