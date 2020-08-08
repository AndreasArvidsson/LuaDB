const coll = db.getCollection("test");
const size = 100000;

function run (name, callback) {
    const t0 = Date.now();
	callback();
	const t1 = Date.now();
	print(name, t1 - t0, "ms");
}

// Clean state
coll.remove({})

print("\nSize\t\t\t", size, "\n");

run("Insert {}\t\t", () => {
    for (let i = 0; i < size; ++i) {
        coll.insert({});
    }
})

const doc = {
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

//Compensate for insert table and insert table inline
for (let i = 0; i < size * 2; ++i) {
    coll.insert(doc);
}

run("Insert document\t\t", () => {
    const doc = {
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
    for (let i = 0; i < size; ++i) {
        coll.insert(doc);
    }
})

run("Insert document inline\t", () => {
    for (let i = 0; i < size; ++i) {
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
})

run("insertMany\t\t", () => {
    const doc = {
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
    const docs = [];
    for (let i = 0; i < size; ++i) {
        docs[i] = doc;
    }
    coll.insertMany(docs);
})

run("count\t\t\t", () => {
    coll.count({a: -5});
})

run("cursor.next\t\t", () => {
    const cursor = coll.find({});
    while (cursor.hasNext()) {
        cursor.next();
    }
})

run("forEach\t\t\t", () => {
    coll.find({}).forEach(doc => {
    });
})

//Have no iterator

run("toArray\t\t\t", () => {
    coll.find({}).toArray();
})

run("remove\t\t\t", () => {
    coll.remove({});
})

run("Empty loop\t\t", () => {
    for (var i = 0; i < size * 1000; ++i) { }
})

run("Function call\t\t", () => {
    function func() { }
    for (var i = 0; i < size * 1000; ++i) {
        func();
    }
})