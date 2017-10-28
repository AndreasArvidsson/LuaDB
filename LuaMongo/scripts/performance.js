
var coll = db.getCollection("test");
var size = 100000;

var doc = {
    a: -5,
    b: 7,
    c: -2.5,
    d: 7.9,
    e: true,
    f: false,
    g: "hej",
    h: null,
    i: [1, 2, 3, "hello", null, true, false, -1, 1, 5.5, 2.74],
    j: { a: 2 },
    k: {}
};

// Clean state
coll.remove({})

print("\nSize\t\t\t", size, "\n");

var t1 = new Date().getTime();
for (var i = 0; i < size; ++i) {
    coll.insert({});
}
print("Insert {}\t\t", (new Date().getTime() - t1), "ms");

//Compensate for insert table
for (var i = 0; i < size; ++i) {
    coll.insert(doc);
}

//Compensate for insert table inline
for (var i = 0; i < size; ++i) {
    coll.insert(doc);
}

var t1 = new Date().getTime();
for (var i = 0; i < size; ++i) {
    coll.insert(doc);
}
print("Insert document\t\t", (new Date().getTime() - t1), "ms");

var t1 = new Date().getTime();
for (var i = 0; i < size; ++i) {
    coll.insert({
        a: -5,
        b: 7,
        c: -2.5,
        d: 7.9,
        e: true,
        f: false,
        g: "hej",
        h: null,
        i: [1, 2, 3, "hello", null, true, false, -1, 1, 5.5, 2.74],
        j: { a: 2 },
        k: {}
    });
}
print("Insert doc inline\t", (new Date().getTime() - t1), "ms");

var t1 = new Date().getTime();
var docs = [];
for (var i = 0; i < size; ++i) {
    docs[i] = doc;
}
coll.insertMany(docs);
print("InsertMany doc\t\t", (new Date().getTime() - t1), "ms");

var t1 = new Date().getTime();
coll.count({a: -5});
print("Count\t\t\t", (new Date().getTime() - t1), "ms");

var t1 = new Date().getTime();
var cursor = coll.find({});
while (cursor.hasNext()) {
    cursor.next();
}
print("Next\t\t\t", (new Date().getTime() - t1), "ms");

var t1 = new Date().getTime();
coll.find({}).forEach(function(doc) {});
print("ForEach\t\t\t", (new Date().getTime() - t1), "ms");

//Have no iterator

var t1 = new Date().getTime();
coll.find({}).toArray();
print("toArray\t\t\t", (new Date().getTime() - t1), "ms");

var t1 = new Date().getTime();
coll.remove({});
print("Remove\t\t\t", (new Date().getTime() - t1), "ms");

var t1 = new Date().getTime();
for (var i = 0; i < size * 1000; ++i) { }
print("Empty loop\t\t", (new Date().getTime() - t1), "ms");

function func() { }
var t1 = new Date().getTime();
for (var i = 0; i < size * 1000; ++i) {
    func();
}
print("Function call\t\t", (new Date().getTime() - t1), "ms");