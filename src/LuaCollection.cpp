#include "LuaCollection.h"
#include "MongoDB.h"
#include "LuaParserUtil.h" 
#include "LuaCursor.h"
#include "LuaBsonTypes.h"

#define COLLECTION_TABLE_NAME "MongoCollection"

/*************************************
*************** STATIC ***************
**************************************/

void LuaCollection::lua_push(lua_State *L, LuaCollection *collection) {
	*(LuaCollection**)(lua_newuserdata(L, sizeof(LuaCollection*))) = collection;
	if (luaL_newmetatable(L, COLLECTION_TABLE_NAME)) {
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_setStr(L, "Protected", "__metatable");
		lua_setFuncs(L, _bindFuncs);
	}
	lua_setmetatable(L, -2);
}

/*************************************
*************** PUBLIC ***************
**************************************/

LuaCollection::LuaCollection(MongoCollection *pMongoCollection) {
	_pMongoCollection = pMongoCollection;
}

LuaCollection::~LuaCollection() {
}

/*************************************
*************** PRIVATE **************
**************************************/

LuaCollection* LuaCollection::lua_get(lua_State *L, const int index) {
	return *reinterpret_cast<LuaCollection**>(luaL_checkudata(L, index, COLLECTION_TABLE_NAME));
}

/*************************************
************** LUA BIND **************
**************************************/

const luaL_reg LuaCollection::_bindFuncs[] = {
	{ "__tostring", lua_toString },
	{ "help", lua_help },
	{ "getName", lua_getName },
	{ "getFullName", lua_getFullName },
	{ "find", lua_find },
	{ "findOne", lua_findOne },
	{ "aggregate", lua_aggregate },
	{ "count", lua_count },
	{ "distinct", lua_distinct },
	{ "insert", lua_insert },
	{ "insertMany", lua_insertMany },
	{ "save", lua_save },
	{ "update", lua_update },
	{ "remove", lua_remove },
	{ "renameCollection", lua_renameCollection },
	{ "runCommand", lua_runCommand },
	{ "drop", lua_drop },
	{ "createIndex", lua_createIndex },
	{ "dropIndex", lua_dropIndex },
	{ "createTestData", lua_createTestData },
	{ NULL, NULL }
};

int LuaCollection::lua_toString(lua_State *L) {
	LuaCollection *pColl = lua_get(L, 1);
	lua_pushstring(L, pColl->_pMongoCollection->getFullName().c_str());
	return 1;
}

int LuaCollection::lua_help(lua_State *L) {
	printf("Collection functions:\n");
	for (int i = 0; _bindFuncs[i].name; ++i) {
		if (_bindFuncs[i].name[0] != '_' && strcmp(_bindFuncs[i].name, "help")) {
			printf("\t%s()\n", _bindFuncs[i].name);
		}
	}
	return 0;
}

int LuaCollection::lua_getName(lua_State *L) {
	lua_pushstring(L, lua_get(L, 1)->_pMongoCollection->getName().c_str());
	return 1;
}

int LuaCollection::lua_getFullName(lua_State *L) {
	lua_pushstring(L, lua_get(L, 1)->_pMongoCollection->getFullName().c_str());
	return 1;
}

int LuaCollection::lua_renameCollection(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	const char *newName = lua_tostring(L, 2);
	bool dropExisting = lua_gettop(L) > 2 ? lua_toboolean(L, 3) : false;

	bson_error_t error;
	if (!pCollection->_pMongoCollection->renameCollection(newName, dropExisting, &error)) {
		luaL_error(L, "Rename failure: %s\n", error.message);
	}

	LuaBsonTypes::lua_pushOkResult(L);
	return 1;
}

int LuaCollection::lua_find(lua_State *L) {
	const int count = lua_gettop(L);
	LuaCollection *pCollection = lua_get(L, 1);
	bson_t *pProjection, *pQuery = bson_new();
	if (count > 1) {
		LuaParserUtil::luaToBson(L, pQuery, 2);
	}
	if (count > 2) {
		pProjection = bson_new();
		LuaParserUtil::luaToBson(L, pProjection, 3);
	}
	else {
		pProjection = nullptr;
	}

	mongoc_cursor_t *pMongoCursor = pCollection->_pMongoCollection->find(pQuery, pProjection);

	bson_destroy(pQuery);
	if (pProjection) {
		bson_destroy(pProjection);
	}

    LuaCursor::lua_push(L, pCollection->_pMongoCollection, pMongoCursor);

	return 1;
}

int LuaCollection::lua_findOne(lua_State *L) {
	const int count = lua_gettop(L);
	LuaCollection *pCollection = lua_get(L, 1);
	bson_t *pProjection, *pQuery = bson_new();
	if (count > 1) {
		LuaParserUtil::luaToBson(L, pQuery, 2);
	}
	if (count > 2) {
		pProjection = bson_new();
		LuaParserUtil::luaToBson(L, pProjection, 3);
	}
	else {
		pProjection = nullptr;
	}

	mongoc_cursor_t *pMongoCursor = pCollection->_pMongoCollection->find(pQuery, pProjection);

	//Has next. Return it.
	const bson_t *pReply;
	if (mongoc_cursor_next(pMongoCursor, &pReply)) {
		LuaParserUtil::bsonToLua(pReply, L);
	}
	//No next. Check for errors.
	else {
		bson_error_t error;
		if (mongoc_cursor_error(pMongoCursor, &error)) {
			luaL_error(L, "FindOne failure: %s\n", error.message);
		}
	}

	mongoc_cursor_destroy(pMongoCursor);
	bson_destroy(pQuery);
	if (pProjection) {
		bson_destroy(pProjection);
	}
	return pReply ? 1 : 0;
}

int LuaCollection::lua_aggregate(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	bson_t *pOptions, *pPipeline = bson_new();
	LuaParserUtil::luaToBson(L, pPipeline, 2);
	if (lua_gettop(L) > 2) {
		pOptions = bson_new();
		LuaParserUtil::luaToBson(L, pOptions, 3);
	}
	else {
		pOptions = nullptr;
	}

	mongoc_cursor_t *pMongoCursor = pCollection->_pMongoCollection->aggregate(pPipeline, pOptions);

	bson_destroy(pPipeline);
	if (pOptions) {
		bson_destroy(pOptions);
	}

    LuaCursor::lua_push(L, pCollection->_pMongoCollection, pMongoCursor);

	return 1;
}

int LuaCollection::lua_count(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	bson_t *pQuery = bson_new();
	if (lua_gettop(L) > 1) {
		LuaParserUtil::luaToBson(L, pQuery, 2);
	}

	bson_error_t error;
	int64_t count = pCollection->_pMongoCollection->count(pQuery, &error);
	if (count < 0) {
		luaL_error(L, "Count failure: %s\n", error.message);
	}

	bson_destroy(pQuery);
	lua_pushinteger(L, count);
	return 1;
}

int LuaCollection::lua_distinct(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	const char* key = lua_tostring(L, 2);
	bson_t *pQuery = bson_new();
	if (lua_gettop(L) > 2) {
		LuaParserUtil::luaToBson(L, pQuery, 3);
	}

	bson_t reply;
	bson_error_t error;
	if(!pCollection->_pMongoCollection->distinct(key, pQuery, &reply, &error)) {
		luaL_error(L, "Distinct failure: %s\n", error.message);
	}

	LuaParserUtil::bsonSubToLua(&reply, L, "values");
	bson_destroy(pQuery);
	bson_destroy(&reply);
	return 1;
}

int LuaCollection::lua_insert(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	bson_t *pDocument = bson_new();
	LuaParserUtil::luaToBson(L, pDocument, 2);

	bson_t *pWR = bson_new();
	bson_error_t error;
	if (!pCollection->_pMongoCollection->insert(pDocument, pWR, &error)) {
		luaL_error(L, "Insert failure: %s\n", error.message);
	}

	LuaBsonTypes::lua_pushWriteResult(L, pWR);
	bson_destroy(pDocument);
	bson_destroy(pWR);
	return 1;
}

int LuaCollection::lua_insertMany(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	mongoc_bulk_operation_t  *pBulk = pCollection->_pMongoCollection->createBulkOperation();
	bson_t *pDocument = bson_new();
	for (lua_pushnil(L); lua_next(L, 2); lua_pop(L, 1)) {
		LuaParserUtil::luaToBson(L, pDocument, -1);
		mongoc_bulk_operation_insert_with_opts(pBulk, pDocument, nullptr, nullptr);
		bson_reinit(pDocument);
	}

	bson_t reply;
	bson_error_t error;
	if (!pCollection->_pMongoCollection->executeBulkOperation(pBulk, &reply, &error)) {
		luaL_error(L, "Insert many failure: %s\n", error.message);
	}

	bson_t *pWR = bson_new();
	bson_iter_t it;
	bson_append_double(pWR, "nInserted", 9, bson_iter_init_find(&it, &reply, "nInserted") ? bson_iter_int32(&it) : 0);
	LuaBsonTypes::lua_pushWriteResult(L, pWR);
	bson_destroy(pDocument);
	bson_destroy(&reply);
	bson_destroy(pWR);
	return 1;
}

int LuaCollection::lua_save(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	bson_t *pDocument = bson_new();
	LuaParserUtil::luaToBson(L, pDocument, 2);
	
	bson_t *pWR = bson_new();
	bson_error_t error;
	if (!pCollection->_pMongoCollection->save(pDocument, pWR, &error)) {
		luaL_error(L, "Save failure: %s\n", error.message);
	}

	LuaBsonTypes::lua_pushWriteResult(L, pWR);
	bson_destroy(pDocument);
	bson_destroy(pWR);
	return 1;
}

int LuaCollection::lua_update(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	bson_t *pQuery = bson_new();
	LuaParserUtil::luaToBson(L, pQuery, 2);
	bson_t *pUpdate = bson_new();
	LuaParserUtil::luaToBson(L, pUpdate, 3);
	bson_t *pOptions;
	if (lua_gettop(L) > 3) {
		pOptions = bson_new();
		LuaParserUtil::luaToBson(L, pOptions, 4);
	}
	else {
		pOptions = nullptr;
	}

	bson_t *pWR = bson_new();
	bson_error_t error;
	if (!pCollection->_pMongoCollection->update(pQuery, pUpdate, pOptions, pWR, &error)) {
		luaL_error(L, "Update failure: %s\n", error.message);
	}

	LuaBsonTypes::lua_pushWriteResult(L, pWR);
	bson_destroy(pQuery);
	bson_destroy(pUpdate);
	bson_destroy(pWR);
	if (pOptions) {
		bson_destroy(pOptions);
	}
	return 1;
}

int LuaCollection::lua_remove(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	int count = lua_gettop(L);
	bson_t *pQuery = bson_new();
	if (count > 1) {
		LuaParserUtil::luaToBson(L, pQuery, 2);
	}
	bool justOne = count > 2 ? lua_toboolean(L, 3) : false;

	bson_t *pWR = bson_new();
	bson_error_t error;
	if (!pCollection->_pMongoCollection->remove(pQuery, justOne, pWR, &error)) {
		luaL_error(L, "Remove failure: %s\n", error.message);
	}

	LuaBsonTypes::lua_pushWriteResult(L, pWR);
	bson_destroy(pQuery);
	bson_destroy(pWR);
	return 1;
}

int LuaCollection::lua_runCommand(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	bson_t *pCommand = bson_new();
	LuaParserUtil::luaToBson(L, pCommand, 2);
	
	bson_t reply;
	bson_error_t error;
	if (pCollection->_pMongoCollection->runCommand(pCommand, &reply, &error)) {
		LuaParserUtil::bsonToLua(&reply, L);
	}
	else {
		luaL_error(L, "RunCommand failure: %s\n", error.message);
	}

	bson_destroy(pCommand);
	bson_destroy(&reply);
	return 1;
}

int LuaCollection::lua_drop(lua_State *L) {
	bson_error_t error;
	if (!lua_get(L, 1)->_pMongoCollection->drop(&error)) {
		luaL_error(L, "Drop collection failure: %s\n", error.message);
	}
	LuaBsonTypes::lua_pushOkResult(L);
	return 1;
}

int LuaCollection::lua_createIndex(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	bson_t *pOptions, *pKeys = bson_new();
	LuaParserUtil::luaToBson(L, pKeys, 2);
	if (lua_gettop(L) > 2) {
		pOptions = bson_new();
		LuaParserUtil::luaToBson(L, pOptions, 3);
	}
	else {
		pOptions = nullptr;
	}

	bson_t reply;
	bson_error_t error;
	if (!lua_get(L, 1)->_pMongoCollection->createIndex(pKeys, pOptions, &reply, &error)) {
		luaL_error(L, "Create index failure: %s\n", error.message);
	}

	LuaParserUtil::bsonToLua(&reply, L);
	bson_destroy(pKeys);
	bson_destroy(&reply);
	if (pOptions) {
		bson_destroy(pOptions);
	}
	return 1;
}

int LuaCollection::lua_dropIndex(lua_State *L) {
	bson_error_t error;
	if (!lua_get(L, 1)->_pMongoCollection->dropIndex(luaL_checkstring(L, 2), &error)) {
		luaL_error(L, "Drop index failure: %s\n", error.message);
	}
	LuaBsonTypes::lua_pushOkResult(L);
	return 1;
}

int LuaCollection::lua_createTestData(lua_State *L) {
	LuaCollection *pCollection = lua_get(L, 1);
	size_t count = 100000;
	if (lua_gettop(L) > 1) {
		count = lua_tointeger(L, 2);
	}
	//bson_decimal128_t dec;
	//bson_decimal128_from_string("123.5", &dec);
	//const uint8_t raw[5] = { 'a', 'b','c',1,5 };
	//bson_oid_t oid;
	//bson_t *scope = BCON_NEW("name", "myScope");
	//bson_oid_init(&oid, NULL);
	//bson_t *pDocument = BCON_NEW(
	//	"double", BCON_DOUBLE(123.4),
	//	"utf8", "this is my string",
	//	"document", BCON_DOCUMENT(scope),
	//	"array", "[", "a", BCON_BOOL(false), BCON_DOUBLE(5), "]",
	//	"binary", BCON_BIN(BSON_SUBTYPE_BINARY, raw, sizeof raw),
	//	"binaryfunc", BCON_BIN(BSON_SUBTYPE_FUNCTION, raw, sizeof raw),
	//	"binarydep", BCON_BIN(BSON_SUBTYPE_BINARY_DEPRECATED, raw, sizeof raw),
	//	"binaryuuiddep", BCON_BIN(BSON_SUBTYPE_UUID_DEPRECATED, raw, sizeof raw),
	//	"binaryuuid", BCON_BIN(BSON_SUBTYPE_UUID, raw, sizeof raw),
	//	"binarymd5", BCON_BIN(BSON_SUBTYPE_MD5, raw, sizeof raw),
	//	"binaryuser", BCON_BIN(BSON_SUBTYPE_USER, raw, sizeof raw),
	//	"undefined", BCON_UNDEFINED,
	//	"oid", BCON_OID(&oid),
	//	"bool", BCON_BOOL(true),
	//	"datetime", BCON_DATE_TIME(12345678),
	//	"null", BCON_NULL,
	//	"regex", BCON_REGEX("^hello", "i"),
	//	"codewscope", BCON_CODEWSCOPE("var a = 1;", scope),
	//	"dbpointer", BCON_DBPOINTER("test.test", &oid),
	//	"code", BCON_CODE("var a = function() {}"),
	//	"symbol", BCON_SYMBOL("my_symbol"),
	//	"int32", BCON_INT32(1234),
	//	"timestamp", BCON_TIMESTAMP(1234, 4567),
	//	"int64", BCON_INT64(4321),
	//	"maxkey", BCON_MAXKEY,
	//	"minkey", BCON_MINKEY,
	//	"decimal128", BCON_DECIMAL128(&dec)
	//);

	const char *str = "this is a string";
	bson_decimal128_t dec;
	bson_decimal128_from_string("123.5", &dec);
	const uint8_t *raw = (const uint8_t*)str;
	bson_oid_t oid;
	bson_t *scope = BCON_NEW("name", str);
	bson_oid_init(&oid, NULL);
	bson_t *pDocument = BCON_NEW(
		"double", BCON_DOUBLE(123.4),
		"utf8", str,
		"document", BCON_DOCUMENT(scope),
		"array", "[", "a", BCON_BOOL(false), BCON_DOUBLE(5), "]",
		"binary", BCON_BIN(BSON_SUBTYPE_BINARY, raw, 17),
		"binaryfunc", BCON_BIN(BSON_SUBTYPE_FUNCTION, raw, 17),
		"binarydep", BCON_BIN(BSON_SUBTYPE_BINARY_DEPRECATED, raw, 17),
		"binaryuuiddep", BCON_BIN(BSON_SUBTYPE_UUID_DEPRECATED, raw, 17),
		"binaryuuid", BCON_BIN(BSON_SUBTYPE_UUID, raw, 17),
		"binarymd5", BCON_BIN(BSON_SUBTYPE_MD5, raw, 17),
		"binaryuser", BCON_BIN(BSON_SUBTYPE_USER, raw, 17),
		"undefined", BCON_UNDEFINED,
		"oid", BCON_OID(&oid),
		"bool", BCON_BOOL(true),
		"datetime", BCON_DATE_TIME(12345678),
		"null", BCON_NULL,
		"regex", BCON_REGEX(str, "i"),
		"codewscope", BCON_CODEWSCOPE(str, scope),
		"dbpointer", BCON_DBPOINTER(str, &oid),
		"code", BCON_CODE(str),
		"symbol", BCON_SYMBOL(str),
		"int32", BCON_INT32(1234),
		"timestamp", BCON_TIMESTAMP(1234, 4567),
		"int64", BCON_INT64(4321),
		"maxkey", BCON_MAXKEY,
		"minkey", BCON_MINKEY
		//"decimal128", BCON_DECIMAL128(&dec)
	);

	//bson_t *pDocument = BCON_NEW(
	//	"double1", BCON_DOUBLE(123.4),
	//	"double2", BCON_DOUBLE(5),
	//	"double3", BCON_DOUBLE(-7),
	//	"utf81", "this is my string",
	//	"document", BCON_DOCUMENT(scope),
	//	"array", "[", "a", BCON_BOOL(false), BCON_DOUBLE(5), "]",
	//	"undefined", BCON_UNDEFINED,
	//	"bool", BCON_BOOL(true),
	//	"null", BCON_NULL,
	//	"utf82", "this is my other string"
	//);

	mongoc_bulk_operation_t  *pBulk = pCollection->_pMongoCollection->createBulkOperation();
	for (int i = 0; i < count; ++i) {
		mongoc_bulk_operation_insert_with_opts(pBulk, pDocument, nullptr, nullptr);
	}
	bson_t reply;
	bson_error_t error;
	if (!pCollection->_pMongoCollection->executeBulkOperation(pBulk, &reply, &error)) {
		luaL_error(L, "Insert many failure: %s\n", error.message);
	}

	bson_t *pWR = bson_new();
	bson_iter_t it;
	bson_append_double(pWR, "nInserted", 9, bson_iter_init_find(&it, &reply, "nInserted") ? bson_iter_int32(&it) : 0);
	LuaBsonTypes::lua_pushWriteResult(L, pWR);
	bson_destroy(pDocument);
	bson_destroy(&reply);
	bson_destroy(pWR);
	return 1;
}