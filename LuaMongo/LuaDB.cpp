#include "LuaDB.h"
#include "MongoDB.h"
#include "LuaParserUtil.h"
#include "LuaBsonTypes.h"

#define DB_TABLE_NAME "MongoDB"

/*************************************
*************** STATIC ***************
**************************************/

void LuaDB::lua_push(lua_State *L, LuaDB *pDB) {
	*(LuaDB**)(lua_newuserdata(L, sizeof(LuaDB*))) = pDB;
	if (luaL_newmetatable(L, DB_TABLE_NAME)) {
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_setStr(L, "Protected", "__metatable");
		lua_setFuncs(L, _bindFuncs);
	}
	lua_setmetatable(L, -2);
}

void LuaDB::lua_registerDB(lua_State *L, LuaDB *pDB) {
	lua_push(L, pDB);
	lua_setglobal(L, "db");
}

/*************************************
*************** PUBLIC ***************
**************************************/

LuaDB::LuaDB(MongoDB *pMongoDB) {
	_pMongoDB = pMongoDB;
}

LuaDB::~LuaDB() {
	for (auto it : _collections) {
		delete it.second;
	}
}

LuaCollection* LuaDB::getCollection(const String name) {
	std::unordered_map<String, LuaCollection*>::const_iterator found = _collections.find(name);

	//Already have this collection.
	if (found != _collections.end()) {
		return found->second;
	}

	//Create new collection
	MongoCollection *pMongoCollection = _pMongoDB->getCollection(name);
	LuaCollection *pColl = new LuaCollection(pMongoCollection);
	_collections[name] = pColl;
	return pColl;
}

void LuaDB::showCollections(lua_State *L) {
	std::vector<String> names;
	bson_error_t error;
	if (!_pMongoDB->getCollectionNames(names, &error)) {
		luaL_error(L, "Show collections failure: %s\n", error.message);
	}
	for (int i = 0; i < names.size(); ++i) {
		printf("%s\n", names[i].c_str());
	}
}

/*************************************
************** PRIVATE ***************
**************************************/

LuaDB* LuaDB::lua_get(lua_State *L, const int index) {
	return *reinterpret_cast<LuaDB**>(luaL_checkudata(L, index, DB_TABLE_NAME));
}

/*************************************
************** LUA BIND **************
**************************************/

const luaL_reg LuaDB::_bindFuncs[] = {
	{ "__index", lua_index },
	{ "__tostring", lua_getName },
	{ "help", lua_help },
	{ "getName", lua_getName },
	{ "getCollection", lua_getCollection },
	{ "hasCollection", lua_hasCollection },
	{ "createCollection", lua_createCollection },
	{ "getCollectionNames", lua_getCollectionNames },
	{ "showCollections", lua_showCollections },
	{ "runCommand", lua_runCommand },
	{ "drop", lua_drop },
	{ NULL, NULL }
};

int LuaDB::lua_index(lua_State *L) {
	LuaDB *pDB = lua_get(L, 1);
	if (luaL_getmetafield(L, 1, luaL_checkstring(L, 2))) {
		return 1;
	}
	return pDB->lua_getCollection(L);
}

int LuaDB::lua_help(lua_State *L) {
	printf("DB functions:\n");
	for (int i = 0; _bindFuncs[i].name; ++i) {
		if (_bindFuncs[i].name[0] != '_' && strcmp(_bindFuncs[i].name, "help")) {
			printf("\t%s()\n", _bindFuncs[i].name);
		}
	}
	return 0;
}

int LuaDB::lua_getName(lua_State *L) {
	lua_pushstring(L, lua_get(L, 1)->_pMongoDB->getName().c_str());
	return 1;
}

int LuaDB::lua_getCollection(lua_State *L) {
	LuaDB *pDB = lua_get(L, 1);
	LuaCollection* pCollection = pDB->getCollection(luaL_checkstring(L, 2));
	LuaCollection::lua_push(L, pCollection);
	return 1;
}

int LuaDB::lua_hasCollection(lua_State *L) {
	bson_error_t error;
	if (lua_get(L, 1)->_pMongoDB->hasCollection(luaL_checkstring(L, 2), &error)) {
		lua_pushboolean(L, true);
	}
	else if (error.code){
		luaL_error(L, "Has collection failure: %s\n", error.message);
	}
	else {
		lua_pushboolean(L, false);
	}
	return 1;
}

int LuaDB::lua_getCollectionNames(lua_State *L) {
	LuaDB *pDB = lua_get(L, 1);
	std::vector<String> names;
	bson_error_t error;
	if (!pDB->_pMongoDB->getCollectionNames(names, &error)) {
		luaL_error(L, "Get collection names failure: %s\n", error.message);
	}
	lua_createtable(L, (int)names.size(), 0);
	for (int i = 0; i < names.size(); ++i) {
		lua_pushlstring(L, names[i].c_str(), names[i].size());
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

int LuaDB::lua_showCollections(lua_State *L) {
	lua_get(L, 1)->showCollections(L);
	return 0;
}

int LuaDB::lua_createCollection(lua_State *L) {
	int count = lua_gettop(L);
	const char *pName = luaL_checkstring(L, 2);
	bson_t *pOptions;
	if (count > 2) {
		pOptions = bson_new();
		LuaParserUtil::luaToBson(L, pOptions, 3);
	}
	else {
		pOptions = nullptr;
	}

	bson_error_t error;
	if (!lua_get(L, 1)->_pMongoDB->createCollection(pName, pOptions, &error)) {
		luaL_error(L, "Create collection failure: %s\n", error.message);
	}

	if (pOptions) {
		bson_destroy(pOptions);
	}
	LuaBsonTypes::lua_pushOkResult(L);
	return 1;
}

int LuaDB::lua_runCommand(lua_State *L) {
	LuaDB *pDB = lua_get(L, 1);
	bson_t *pCommand = bson_new();
	LuaParserUtil::luaToBson(L, pCommand, 2);
	
	bson_t *pReply = bson_new();
	bson_error_t error;
	if (pDB->_pMongoDB->runCommand(pCommand, pReply, &error)) {
		LuaParserUtil::bsonToLua(pReply, L);
	}
	else {
		luaL_error(L, "Run command failure: %s\n", error.message);
	}

	bson_destroy(pCommand);
	bson_destroy(pReply);
	return 1;
}

int LuaDB::lua_drop(lua_State *L) {
	bson_error_t error;
	if (!lua_get(L, 1)->_pMongoDB->drop(&error)) {
		luaL_error(L, "Drop database failure: %s\n", error.message);
	}
	LuaBsonTypes::lua_pushOkResult(L);
	return 1;
}