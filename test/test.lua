
function getAssertValue(v)
	if type(v) == "nil" then
		return "nil"
	end
	if type(v) == "boolean" then
		if v then
			return "true"
		end
		return "false"
	end
	return v
end

function assertEqual(a, b, msg)
	local valA = getAssertValue(a)
	local valB = getAssertValue(b)
	if (valA ~= valB) then
		if msg then
			msg = msg..": "
		else
			msg = ""
		end
		assert(false, msg.."Expected '"..valA.."', found '"..valB.."'")
	end	
end

require("test_bsonTypes")
require("test_crud")