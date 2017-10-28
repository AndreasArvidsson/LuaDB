
-- DOCUMENT

local doc = Document({"a", 101, "b", 102, "c", 103, "d", 104})
doc:delete("b")
doc.c = nil
doc.b = 105
local json = tojson(doc, true)

assertEqual([[{
    "a" : 101,
    "c" : undefined,
    "d" : 104,
    "b" : 105
}]], json);

for k,v in doc:pairs() do
	assertEqual(doc[k], v)
end


-- ARRAY

local arr = Array({1, "hej", true, false, 5.5});
arr[3] = nil
local json = tojson(arr, true)

assertEqual([=[[
    1,
    "hej",
    false,
    5.5
]]=], json)


-- NULL / UNDEF

assertEqual('null', tojson(null))
assertEqual(true, null == null)
assertEqual('undefined', tojson(nil))


-- NUMBER INT

assertEqual('NumberInt(0)', tojson(NumberInt()))
assertEqual('NumberInt(5)', tojson(NumberInt(5)))
assertEqual('NumberInt(5)', tojson(NumberInt("5")))
assertEqual('NumberInt(-5)', tojson(NumberInt(-5)))
assertEqual('NumberInt(-2147483648)', tojson(NumberInt(2147483648)))
assertEqual('NumberInt(0)', tojson(NumberInt(21474836480)))
assertEqual('NumberInt(5)', tojson(NumberInt(5.5)))
local numInt1 = NumberInt(5)
local numInt2 = numInt1
assertEqual('NumberInt(5)', tojson(numInt2))
assertEqual(-5, -NumberInt(5))
assertEqual(6.5, NumberInt(5) + 1.5)
assertEqual(3.5, NumberInt(5) - 1.5)
assertEqual(10, NumberInt(5) * 2)
assertEqual(2.5, NumberInt(5) / 2)
assertEqual(1, NumberInt(5) % 2)
assertEqual(25, NumberInt(5) ^ 2)
assertEqual("value: 5", "value: "..NumberInt(5))
assertEqual(true, NumberInt(5) == NumberInt(5))
assertEqual(false, NumberInt(5) == NumberInt(4))
assertEqual(true, NumberInt(5) ~= NumberInt(4))
assertEqual(false, NumberInt(5) ~= NumberInt(5))
assertEqual(true, NumberInt(4) < NumberInt(5))
assertEqual(false, NumberInt(5) < NumberInt(5))
assertEqual(true, NumberInt(4) <= NumberInt(5))
assertEqual(false, NumberInt(4) <= NumberInt(3))
assertEqual(true, NumberInt(6) > NumberInt(5))
assertEqual(false, NumberInt(5) > NumberInt(5))
assertEqual(true, NumberInt(5) >= NumberInt(5))
assertEqual(false, NumberInt(4) >= NumberInt(5))


-- NUMBER LONG

assertEqual('NumberLong(0)', tojson(NumberLong()))
assertEqual('NumberLong(5)', tojson(NumberLong(5)))
assertEqual('NumberLong(5)', tojson(NumberLong("5")))
assertEqual('NumberLong(2147483647)', tojson(NumberLong(2147483647)))
assertEqual('NumberLong(-2147483647)', tojson(NumberLong(-2147483647)))
assertEqual('NumberLong("2147483648")', tojson(NumberLong(2147483648)))
assertEqual('NumberLong("-2147483648")', tojson(NumberLong(-2147483648)))
assertEqual('NumberLong("2555555000005")', tojson(NumberLong("2555555000005")))
local numLong1 = NumberLong(5)
local numLong2 = numLong1
assertEqual('NumberLong(5)', tojson(numLong2))
assertEqual(-5, -NumberLong(5))
assertEqual(6.5, NumberLong(5) + 1.5)
assertEqual(3.5, NumberLong(5) - 1.5)
assertEqual(10, NumberLong(5) * 2)
assertEqual(2.5, NumberLong(5) / 2)
assertEqual(1, NumberLong(5) % 2)
assertEqual(25, NumberLong(5) ^ 2)
assertEqual("value: 5", "value: "..NumberLong(5))
assertEqual(true, NumberLong(5) == NumberLong("5"))
assertEqual(true, NumberLong("5") == NumberLong("5"))
assertEqual(false, NumberLong(5) == NumberLong("6"))
assertEqual(true, NumberLong(5) == NumberLong(5))
assertEqual(false, NumberLong(5) == NumberLong(4))
assertEqual(true, NumberLong(5) ~= NumberLong(4))
assertEqual(false, NumberLong(5) ~= NumberLong(5))
assertEqual(true, NumberLong(4) < NumberLong(5))
assertEqual(false, NumberLong(5) < NumberLong(5))
assertEqual(true, NumberLong(4) <= NumberLong(5))
assertEqual(false, NumberLong(4) <= NumberLong(3))
assertEqual(true, NumberLong(6) > NumberLong(5))
assertEqual(false, NumberLong(5) > NumberLong(5))
assertEqual(true, NumberLong(5) >= NumberLong(5))
assertEqual(false, NumberLong(4) >= NumberLong(5))


-- NUMBER DECIMAL
--[[ TODO constructor not done
assertEqual('NumberDecimal("0")', tojson(NumberDecimal()))
assertEqual('NumberDecimal("0.00000000000000")', tojson(NumberDecimal(0)))
assertEqual('NumberDecimal("5.00000000000000")', tojson(NumberDecimal(5)))
assertEqual('NumberDecimal("5555555.00000000")', tojson(NumberDecimal(5555555)))
assertEqual('NumberDecimal("1000.55000000000")', tojson(NumberDecimal(1000.55)))
assertEqual('NumberDecimal("9999999.50000000")', tojson(NumberDecimal(9999999.4999999999)))
assertEqual('NumberDecimal("0")', tojson(NumberDecimal("0")))
assertEqual('NumberDecimal("5")', tojson(NumberDecimal("5")))
assertEqual('NumberDecimal("1000.55000000000")', tojson(NumberDecimal("1000.55000000000")))
]]

-- OBJECT ID

local id = ObjectId()
assert(id)
id = ObjectId("59e9eb30773004e9f57fc168")
assertEqual('ObjectId("59e9eb30773004e9f57fc168")', tojson(id), "tojson(ObjectId)")
assertEqual('ObjectId("59e9eb30773004e9f57fc168")', id:toString(), "ObjectId toString()")
assertEqual('59e9eb30773004e9f57fc168', id:valueOf(), "ObjectId valueOf()")
assertEqual('ISODate("2017-10-20T12:25:20Z")', tojson(id:getTimestamp()), "ObjectId getTimestamp()")


-- ISO DATE

local isoDate = ISODate(1508504326123)
assertEqual('ISODate("2017-10-20T12:58:46.123Z")', tojson(isoDate))
assertEqual('ISODate("2017-10-20T12:58:46.123Z")', isoDate:toString())
assertEqual(1508504326123, isoDate:valueOf())
assertEqual('ISODate("2017-10-14T12:13:45.123Z")', tojson(ISODate("2017-10-14T12:13:45.123Z")))
assertEqual('ISODate("2017-10-14T12:13:45.123Z")', tojson(ISODate("2017-10-14T12:13:45.123")))
assertEqual('ISODate("2017-10-14T12:13:45.120Z")', tojson(ISODate("2017-10-14T12:13:45.12")))
assertEqual('ISODate("2017-10-14T12:13:45.100Z")', tojson(ISODate("2017-10-14T12:13:45.1")))
assertEqual('ISODate("2017-10-14T12:13:45Z")', tojson(ISODate("2017-10-14T12:13:45.")))
assertEqual('ISODate("2017-10-14T12:13:45Z")', tojson(ISODate("2017-10-14T12:13:45")))
assertEqual('ISODate("2017-10-14T12:13:04Z")', tojson(ISODate("2017-10-14T12:13:4")))
assertEqual('ISODate("2017-10-14T12:13:00Z")', tojson(ISODate("2017-10-14T12:13:")))
assertEqual('ISODate("2017-10-14T12:13:00Z")', tojson(ISODate("2017-10-14T12:13")))
assertEqual('ISODate("2017-10-14T12:01:00Z")', tojson(ISODate("2017-10-14T12:1")))
assertEqual('ISODate("2017-10-14T12:00:00Z")', tojson(ISODate("2017-10-14T12:")))
assertEqual('ISODate("2017-10-14T12:00:00Z")', tojson(ISODate("2017-10-14T12")))
assertEqual('ISODate("2017-10-14T01:00:00Z")', tojson(ISODate("2017-10-14T1")))
assertEqual('ISODate("2017-10-14T00:00:00Z")', tojson(ISODate("2017-10-14T")))
assertEqual('ISODate("2017-10-14T00:00:00Z")', tojson(ISODate("2017-10-14")))
assertEqual('ISODate("2017-10-01T00:00:00Z")', tojson(ISODate("2017-10-1")))
assertEqual('ISODate("2017-01-01T00:00:00Z")', tojson(ISODate("2017-1-1")))
assertEqual('ISODate("1980-01-01T00:00:00Z")', tojson(ISODate("1980-1-1")))


-- REGEXP

assertEqual('/(?:)/', tojson(RegExp()))
assertEqual('/(?:)/', tojson(RegExp("")))
assertEqual('/a/i', tojson(RegExp("a", "i")))



--Write reulst
--Binary / uuid
--print(UUID('0123456789abcdeffedcba9876543210'))

