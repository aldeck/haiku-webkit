if (this.importScripts) {
    importScripts('../../../fast/js/resources/js-test-pre.js');
    importScripts('shared.js');
}

description("Test IndexedDB's key generator behavior.");

function test()
{
    removeVendorPrefixes();

    runTests();
}

var tests = [];
function defineTest(description, verchange, optional) {
  tests.push(
    {
      description: description,
      verchange: verchange,
      optional: optional
    }
  );
}

function runTests() {

    function nextTest() {
        if (!tests.length) {
            finishJSTest();
            return;
        }

        var test = tests.shift();
        debug("");
        debug(test.description);

        evalAndLog("request = indexedDB.deleteDatabase('key-generator')");
        request.onerror = unexpectedErrorCallback;
        request.onsuccess = function () {
            evalAndLog("request = indexedDB.open('key-generator')");
            request.onerror = unexpectedErrorCallback;
            request.onsuccess = function () {
                evalAndLog("db = request.result");
                evalAndLog("request = db.setVersion('1')");
                request.onerror = unexpectedErrorCallback;
                request.onsuccess = function () {
                    evalAndLog("trans = request.result");
                    trans.onabort = unexpectedAbortCallback;
                    test.verchange(db, trans);
                    trans.oncomplete = function () {

                        function finishTest() {
                            evalAndLog("db.close()");
                            nextTest();
                        }

                        if (test.optional) {
                            test.optional(db, finishTest);
                        } else {
                            finishTest();
                        }
                    };
                };
            };
        };
    }

    nextTest();
}

function check(store, key, expected) {
    self.store = store;
    request = evalAndLog("request = store.get(" + JSON.stringify(key) + ")");
    request.onerror = unexpectedErrorCallback;
    request.onsuccess = function (e) {
        self.expected = expected;
        if (JSON.stringify(event.target.result) === JSON.stringify(expected)) {
            testPassed("Got " + JSON.stringify(event.target.result) + " for key: " + JSON.stringify(key));
        } else {
            testFailed("Got " + JSON.stringify(event.target.result) + " for key: " + JSON.stringify(key) +
                " expected: " + JSON.stringify(expected));
        }
    };
}

defineTest(
    'Verify that each object store has an independent key generator.',
    function (db, trans) {
        evalAndLog("store1 = db.createObjectStore('store1', { autoIncrement: true })");
        evalAndLog("store1.put('a')");
        check(store1, 1, 'a');
        evalAndLog("store2 = db.createObjectStore('store2', { autoIncrement: true })");
        evalAndLog("store2.put('a')");
        check(store2, 1, 'a');
        evalAndLog("store1.put('b')");
        check(store1, 2, 'b');
        evalAndLog("store2.put('b')");
        check(store2, 2, 'b');
    }
);

defineTest(
    'Verify that the key generator is not updated if insertion fails',
    function (db, trans) {
        trans.onerror = function(e) { e.preventDefault() };
        evalAndLog("store = db.createObjectStore('store1', { autoIncrement: true })");
        evalAndLog("index = store.createIndex('index1', 'ix', { unique: true })");
        evalAndLog("store.put({ ix: 'a'})");
        check(store, 1, {ix: 'a'});
        evalAndLog("store.put({ ix: 'a'})");
        evalAndLog("store.put({ ix: 'b'})");
        check(store, 2, {ix: 'b'});
    }
);

defineTest(
    'Verify that the key generator is not affected by item removal (delete or clear).',
    function (db, trans) {
        evalAndLog("store = db.createObjectStore('store1', { autoIncrement: true })");
        evalAndLog("store.put('a')");
        check(store, 1, 'a');
        evalAndLog("store.delete(1)");
        evalAndLog("store.put('b')");
        check(store, 2, 'b');
        evalAndLog("store.clear()");
        evalAndLog("store.put('c')");
        check(store, 3, 'c');
        // FIXME: IDBObjectStore.delete(IDBKeyRange) is not yet implemented in Chromium.
        // http://crbug.com/101384
        //evalAndLog("store.delete(IDBKeyRange.lowerBound(0))");
        evalAndLog("store.put('d')");
        check(store, 4, 'd');
    }
);

defineTest(
    'Verify that the key generator is only set if and only if a numeric key greater than the last generated key is used.',
    function (db, trans) {
        evalAndLog("store = db.createObjectStore('store1', { autoIncrement: true })");
        evalAndLog("store.put('a')");
        check(store, 1, 'a');
        evalAndLog("store.put('b', 3)");
        check(store, 3, 'b');
        evalAndLog("store.put('c')");
        check(store, 4, 'c');
        evalAndLog("store.put('d', -10)");
        check(store, -10, 'd');
        evalAndLog("store.put('e')");
        check(store, 5, 'e');
        evalAndLog("store.put('f', 6.00001)");
        check(store, 6.00001, 'f');
        evalAndLog("store.put('g')");
        check(store, 7, 'g');
        evalAndLog("store.put('f', 8.9999)");
        check(store, 8.9999, 'f');
        evalAndLog("store.put('g')");
        check(store, 9, 'g');
        evalAndLog("store.put('h', 'foo')");
        check(store, 'foo', 'h');
        evalAndLog("store.put('i')");
        check(store, 10, 'i');
        evalAndLog("store.put('j', [1000])");
        check(store, [1000], 'j');
        evalAndLog("store.put('k')");
        check(store, 11, 'k');

        // FIXME: Repeat this test, but with a keyPath and inline key.
    }
);

defineTest(
    'Verify that aborting a transaction resets the key generator state.',
    function (db, trans) {
        db.createObjectStore('store', { autoIncrement: true });
    },

    function (db, callback) {
        evalAndLog("trans1 = db.transaction(['store'], IDBTransaction.READ_WRITE)");
        evalAndLog("store_t1 = trans1.objectStore('store')");
        evalAndLog("store_t1.put('a')");
        check(store_t1, 1, 'a');
        evalAndLog("store_t1.put('b')");
        check(store_t1, 2, 'b');

        // Schedule the abort as a task (not run it synchronously)
        store_t1.get(0).onsuccess = function () {
            debug('aborting...');
            evalAndLog("trans1.abort()");
            trans1.onabort = function () {
                debug('aborted!');

                evalAndLog("trans2 = db.transaction(['store'], IDBTransaction.READ_WRITE)");
                evalAndLog("store_t2 = trans2.objectStore('store')");
                evalAndLog("store_t2.put('c')");
                check(store_t2, 1, 'c');
                evalAndLog("store_t2.put('d')");
                check(store_t2, 2, 'd');

                trans2.oncomplete = callback;
            };
        };
    }
);

test();