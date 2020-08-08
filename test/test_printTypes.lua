print("----- Print types -----")

print("hello", "world")
print(1, 55.74, NumberInt(55), NumberLong(22))
print(true, false)
print(nil, null)
print({a=5}, Document({"a", 5}))
print({1, "hello", true}, Array({1, "hello", true}))
print(function() end)
print(ObjectId())
print(ISODate(1508504326123))
print(RegExp("a", "i"))

print("-----------------------\n")