// original test:
// http://mxr.mozilla.org/mozilla2.0/source/dom/indexedDB/test/test_autoIncrement_indexes.html?force=1
// license of original test:
// " Any copyright is dedicated to the Public Domain.
//   http://creativecommons.org/publicdomain/zero/1.0/ "

if (this.importScripts) {
    importScripts('../../../../fast/js/resources/js-test-pre.js');
    importScripts('../../resources/shared.js');
}

description("Test IndexedDB indexes against autoincrementing keys");

function test()
{
    removeVendorPrefixes();

    name = self.location.pathname;
    description = "My Test Database";
    request = evalAndLog("indexedDB.open(name, description)");
    request.onsuccess = openSuccess;
    request.onerror = unexpectedErrorCallback;
}

function openSuccess()
{
    db = evalAndLog("db = event.target.result");

    request = evalAndLog("request = db.setVersion('1')");
    request.onsuccess = setupObjectStore;
    request.onerror = unexpectedErrorCallback;
}

function setupObjectStore()
{
    trans = event.target.result;
    deleteAllObjectStores(db);

    objectStore = evalAndLog("objectStore = db.createObjectStore('foo', { keyPath: 'id', autoIncrement: true });");
    evalAndLog("objectStore.createIndex('first', 'first');");
    evalAndLog("objectStore.createIndex('second', 'second');");
    evalAndLog("objectStore.createIndex('third', 'third');");
    data = evalAndLog("data = { first: 'foo', second: 'foo', third: 'foo' };");
    request = evalAndLog("request = objectStore.add(data);");
    request.onsuccess = setupIndexes;
    request.onerror = unexpectedErrorCallback;
}

function setupIndexes()
{
    key = evalAndLog("key = event.target.result;");
    shouldBeFalse("key == null");
    trans.oncomplete = setVersionComplete;
}

function setVersionComplete()
{
    objectStore = evalAndLog("objectStore = db.transaction('foo').objectStore('foo');");
    first = evalAndLog("first = objectStore.index('first');");
    request = evalAndLog("request = first.get('foo');");
    request.onsuccess = checkFirstIndexAndPrepareSecond;
    request.onerror = unexpectedErrorCallback;
}

function checkFirstIndexAndPrepareSecond()
{
    shouldBe("event.target.result.id", "key");
    second = evalAndLog("second = objectStore.index('second');");
    request = evalAndLog("request = second.get('foo');");
    request.onsuccess = checkSecondIndexAndPrepareThird;
    request.onerror = unexpectedErrorCallback;
}

function checkSecondIndexAndPrepareThird()
{
    shouldBe("event.target.result.id", "key");
    third = evalAndLog("third = objectStore.index('third');");
    request = evalAndLog("request = third.get('foo');");
    request.onsuccess = checkThirdIndex;
    request.onerror = unexpectedErrorCallback;
}

function checkThirdIndex()
{
    shouldBe("event.target.result.id", "key");
    finishJSTest();
}

test();