
print("Test BSON types\n")

-- DOCUMENT

local doc = Document({"a", 101, "b", 102, "c", 103, "d", 104})
doc:delete("b")
doc.c = nil
doc.b = 105
local json = tojson(doc, true)

assertEquals([[{
    "a" : 101,
    "c" : undefined,
    "d" : 104,
    "b" : 105
}]], json);

for k,v in doc:pairs() do
	assertEquals(doc[k], v)
end


-- ARRAY

local arr = Array({1, "hej", true, false, 5.5});
arr[3] = nil
local json = tojson(arr, true)

assertEquals([=[[
    1,
    "hej",
    false,
    5.5
]]=], json)


-- NULL / UNDEF

assertEquals('null', tojson(null))
assertEquals(true, null == null)
assertEquals('undefined', tojson(nil))


-- NUMBER INT

assertEquals('NumberInt(0)', tojson(NumberInt()))
assertEquals('NumberInt(5)', tojson(NumberInt(5)))
assertEquals('NumberInt(5)', tojson(NumberInt("5")))
assertEquals('NumberInt(-5)', tojson(NumberInt(-5)))
assertEquals('NumberInt(-2147483648)', tojson(NumberInt(2147483648)))
assertEquals('NumberInt(0)', tojson(NumberInt(21474836480)))
assertEquals('NumberInt(5)', tojson(NumberInt(5.5)))
local numInt1 = NumberInt(5)
local numInt2 = numInt1
assertEquals('NumberInt(5)', tojson(numInt2))
assertEquals(-5, -NumberInt(5))
assertEquals(6.5, NumberInt(5) + 1.5)
assertEquals(3.5, NumberInt(5) - 1.5)
assertEquals(10, NumberInt(5) * 2)
assertEquals(2.5, NumberInt(5) / 2)
assertEquals(1, NumberInt(5) % 2)
assertEquals(25, NumberInt(5) ^ 2)
assertEquals("value: 5", "value: "..NumberInt(5))
assertEquals(true, NumberInt(5) == NumberInt(5))
assertEquals(false, NumberInt(5) == NumberInt(4))
assertEquals(true, NumberInt(5) ~= NumberInt(4))
assertEquals(false, NumberInt(5) ~= NumberInt(5))
assertEquals(true, NumberInt(4) < NumberInt(5))
assertEquals(false, NumberInt(5) < NumberInt(5))
assertEquals(true, NumberInt(4) <= NumberInt(5))
assertEquals(false, NumberInt(4) <= NumberInt(3))
assertEquals(true, NumberInt(6) > NumberInt(5))
assertEquals(false, NumberInt(5) > NumberInt(5))
assertEquals(true, NumberInt(5) >= NumberInt(5))
assertEquals(false, NumberInt(4) >= NumberInt(5))


-- NUMBER LONG

assertEquals('NumberLong(0)', tojson(NumberLong()))
assertEquals('NumberLong(5)', tojson(NumberLong(5)))
assertEquals('NumberLong(5)', tojson(NumberLong("5")))
assertEquals('NumberLong(2147483647)', tojson(NumberLong(2147483647)))
assertEquals('NumberLong(-2147483647)', tojson(NumberLong(-2147483647)))
assertEquals('NumberLong("2147483648")', tojson(NumberLong(2147483648)))
assertEquals('NumberLong("-2147483648")', tojson(NumberLong(-2147483648)))
assertEquals('NumberLong("2555555000005")', tojson(NumberLong("2555555000005")))
local numLong1 = NumberLong(5)
local numLong2 = numLong1
assertEquals('NumberLong(5)', tojson(numLong2))
assertEquals(-5, -NumberLong(5))
assertEquals(6.5, NumberLong(5) + 1.5)
assertEquals(3.5, NumberLong(5) - 1.5)
assertEquals(10, NumberLong(5) * 2)
assertEquals(2.5, NumberLong(5) / 2)
assertEquals(1, NumberLong(5) % 2)
assertEquals(25, NumberLong(5) ^ 2)
assertEquals("value: 5", "value: "..NumberLong(5))
assertEquals(true, NumberLong(5) == NumberLong("5"))
assertEquals(true, NumberLong("5") == NumberLong("5"))
assertEquals(false, NumberLong(5) == NumberLong("6"))
assertEquals(true, NumberLong(5) == NumberLong(5))
assertEquals(false, NumberLong(5) == NumberLong(4))
assertEquals(true, NumberLong(5) ~= NumberLong(4))
assertEquals(false, NumberLong(5) ~= NumberLong(5))
assertEquals(true, NumberLong(4) < NumberLong(5))
assertEquals(false, NumberLong(5) < NumberLong(5))
assertEquals(true, NumberLong(4) <= NumberLong(5))
assertEquals(false, NumberLong(4) <= NumberLong(3))
assertEquals(true, NumberLong(6) > NumberLong(5))
assertEquals(false, NumberLong(5) > NumberLong(5))
assertEquals(true, NumberLong(5) >= NumberLong(5))
assertEquals(false, NumberLong(4) >= NumberLong(5))


-- NUMBER DECIMAL
--[[ TODO constructor not done
assertEquals('NumberDecimal("0")', tojson(NumberDecimal()))
assertEquals('NumberDecimal("0.00000000000000")', tojson(NumberDecimal(0)))
assertEquals('NumberDecimal("5.00000000000000")', tojson(NumberDecimal(5)))
assertEquals('NumberDecimal("5555555.00000000")', tojson(NumberDecimal(5555555)))
assertEquals('NumberDecimal("1000.55000000000")', tojson(NumberDecimal(1000.55)))
assertEquals('NumberDecimal("9999999.50000000")', tojson(NumberDecimal(9999999.4999999999)))
assertEquals('NumberDecimal("0")', tojson(NumberDecimal("0")))
assertEquals('NumberDecimal("5")', tojson(NumberDecimal("5")))
assertEquals('NumberDecimal("1000.55000000000")', tojson(NumberDecimal("1000.55000000000")))
]]

-- OBJECT ID

local id = ObjectId()
assert(id)
id = ObjectId("59e9eb30773004e9f57fc168")
assertEquals('ObjectId("59e9eb30773004e9f57fc168")', tojson(id), "tojson(ObjectId)")
assertEquals('ObjectId("59e9eb30773004e9f57fc168")', id:toString(), "ObjectId toString()")
assertEquals('59e9eb30773004e9f57fc168', id:valueOf(), "ObjectId valueOf()")
assertEquals('ISODate("2017-10-20T12:25:20Z")', tojson(id:getTimestamp()), "ObjectId getTimestamp()")


-- ISO DATE

local isoDate = ISODate(1508504326123)
assertEquals('ISODate("2017-10-20T12:58:46.123Z")', tojson(isoDate))
assertEquals('ISODate("2017-10-20T12:58:46.123Z")', isoDate:toString())
assertEquals(1508504326123, isoDate:valueOf())
assertEquals('ISODate("2017-10-14T12:13:45.123Z")', tojson(ISODate("2017-10-14T12:13:45.123Z")))
assertEquals('ISODate("2017-10-14T12:13:45.123Z")', tojson(ISODate("2017-10-14T12:13:45.123")))
assertEquals('ISODate("2017-10-14T12:13:45.120Z")', tojson(ISODate("2017-10-14T12:13:45.12")))
assertEquals('ISODate("2017-10-14T12:13:45.100Z")', tojson(ISODate("2017-10-14T12:13:45.1")))
assertEquals('ISODate("2017-10-14T12:13:45Z")', tojson(ISODate("2017-10-14T12:13:45.")))
assertEquals('ISODate("2017-10-14T12:13:45Z")', tojson(ISODate("2017-10-14T12:13:45")))
assertEquals('ISODate("2017-10-14T12:13:04Z")', tojson(ISODate("2017-10-14T12:13:4")))
assertEquals('ISODate("2017-10-14T12:13:00Z")', tojson(ISODate("2017-10-14T12:13:")))
assertEquals('ISODate("2017-10-14T12:13:00Z")', tojson(ISODate("2017-10-14T12:13")))
assertEquals('ISODate("2017-10-14T12:01:00Z")', tojson(ISODate("2017-10-14T12:1")))
assertEquals('ISODate("2017-10-14T12:00:00Z")', tojson(ISODate("2017-10-14T12:")))
assertEquals('ISODate("2017-10-14T12:00:00Z")', tojson(ISODate("2017-10-14T12")))
assertEquals('ISODate("2017-10-14T01:00:00Z")', tojson(ISODate("2017-10-14T1")))
assertEquals('ISODate("2017-10-14T00:00:00Z")', tojson(ISODate("2017-10-14T")))
assertEquals('ISODate("2017-10-14T00:00:00Z")', tojson(ISODate("2017-10-14")))
assertEquals('ISODate("2017-10-01T00:00:00Z")', tojson(ISODate("2017-10-1")))
assertEquals('ISODate("2017-01-01T00:00:00Z")', tojson(ISODate("2017-1-1")))
assertEquals('ISODate("1980-01-01T00:00:00Z")', tojson(ISODate("1980-1-1")))


-- REGEXP

assertEquals('/(?:)/', tojson(RegExp()))
assertEquals('/(?:)/', tojson(RegExp("")))
assertEquals('/a/i', tojson(RegExp("a", "i")))



--Write reulst
--Binary / uuid
--print(UUID('0123456789abcdeffedcba9876543210'))