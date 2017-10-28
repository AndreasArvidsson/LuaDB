
db.test.find({}).forEach(function (doc) {
    printjson(doc);
});



/*
var t1 = new Date().getTime();
var table = {};
for (var i = 0; i < size; ++i) {
    table[i] = i;
}
print("Table insert ", new Date().getTime() - t1)

t1 = new Date().getTime();
for (var i = 0; i < size; ++i) {
    delete table[i];
}
print("Table delete ", new Date().getTime() - t1)



*/

var t1 = new Date().getTime();
db.test.find({}).forEach(function (doc) { });
print("ForEach\t\t\t", (new Date().getTime() - t1), "ms");

