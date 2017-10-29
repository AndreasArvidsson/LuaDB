#pragma once

const char *LuaArray =
	"Array = { \
		__metatable = '[Protected metatable]', \
		__bson = 4 \
	} \
	\
	Array.__index = Array \
	\
	function Array : __tostring() \
		return '[BSON array]' \
	end \
	\
	function Array : new(t) \
		return setmetatable(t or {}, Array) \
	end \
	\
	setmetatable(Array, { \
		__call = function(self, t) \
			return Array:new(t) \
		end, \
		__tostring = function() \
			return '[Array constructor]' \
		end \
	})"
;