#include "LuaManager.h"
#include <limits.h>
#include "File.h"
#include "Date.h"
#include "MongoManager.h"
#include "LuaParserUtil.h"
#include "LuaBsonTypes.h"
#include "LuaDocument.h"
#include "LuaArray.h"
#include "LuaAssert.h"

/*************************************
*************** STATIC ***************
**************************************/

LuaManager *LuaManager::_pInstance = nullptr;

LuaManager* LuaManager::getInstance() {
	if (!_pInstance) {
		_pInstance = new LuaManager();
	}
	return _pInstance;
}

void LuaManager::destroyInstance()  {
	delete _pInstance;
}

/*************************************
*************** PUBLIC ***************
**************************************/

MongoManager* LuaManager::getMongo() const {
	return _pMongo;
}

void LuaManager::useDatabase(const String name) {
	_pDB = getDatabase(name);
	if (_L) {
		//Register global db table
		LuaDB::lua_registerDB(_L, _pDB);
	}
}

const bool LuaManager::isRunning() {
	return _isRunning;
}

void LuaManager::setIsRunning(const bool isRunning) {
	_isRunning = isRunning;
}

void LuaManager::loadFile(const String &path) {
	String str;
	File file(path);
	if (file.exists()) {
		if (file.getData(&str)) {
			if (str.size()) {
				loadString(str.c_str());
			}
		}
		else {
			printf("Error: Can't read from file '%s'", path.c_str());
		}
	}
	else {
		printf("Error: File '%s' doesn't exist", path.c_str());
	}
}

void LuaManager::loadString(const String &str) {
	if (!_L) {
		_L = createNewState();
	}
	doLoadString(_L, str);
}


/*************************************
*************** PRIVATE **************
**************************************/

LuaManager::LuaManager() {
	_pMongo = new MongoManager();
	_isRunning = true;
	_L = nullptr;
	_pDB = nullptr;
}

LuaManager::~LuaManager() {
	if (_L) {
		//Run Lua garbage collector.
		lua_gc(_L, LUA_GCCOLLECT, 0);
		//Close open state.
		lua_close(_L);
	}
	for (auto it : _databases) {
		delete it.second;
	};
	delete _pMongo;
}

lua_State* LuaManager::createNewState()  {
	//Create new lua state
	lua_State *L = luaL_newstate();

	//Register all default libs
	luaL_openlibs(L);

	//Register global functions
	lua_pushglobaltable(L);
	lua_setFuncs(L, _bindFuncs);
	lua_pop(L, 1);

	//Register global bson types
	LuaBsonTypes::lua_registerBsonTypes(L);

	//Register global db table
	if (_pDB) {
		LuaDB::lua_registerDB(L, _pDB);
	}

	//Load libs written i lua code
	doLoadString(L, LuaDocument);
	lua_pushstring(L, BSON_NAME_DOCUMENT);
	lua_getglobal(L, BSON_NAME_DOCUMENT);
	lua_settable(L, LUA_REGISTRYINDEX);

	doLoadString(L, LuaArray);
	lua_pushstring(L, BSON_NAME_ARRAY);
	lua_getglobal(L, BSON_NAME_ARRAY);
	lua_settable(L, LUA_REGISTRYINDEX);

	doLoadString(L, LuaAssert);

	return L;
}

void LuaManager::doLoadString(lua_State *L, const String &str) {
	if (luaL_loadstring(L, str.c_str())) {
		printError(L);
		return;
	}
	if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
		printError(L);
	}
}

void LuaManager::printError(lua_State *L) {
	puts(lua_tostring(L, -1));
	lua_pop(L, 1);
}

LuaDB* LuaManager::getDatabase(const String name) {
	std::unordered_map<String, LuaDB*>::const_iterator found = _databases.find(name);
	//Already have this DB.
	if (found != _databases.end()) {
		return found->second;
	}
	//Create new DB
	MongoDB *pMongoDB = _pMongo->getDatabase(name);
	LuaDB *pLuaDB = new LuaDB(pMongoDB);
	_databases[name] = pLuaDB;
	return pLuaDB;
}

/*************************************
************** LUA BIND **************
**************************************/

const luaL_reg LuaManager::_bindFuncs[] = {
	{ "help", lua_help },
	{ "use", lua_use },
	{ "getSiblingDB", lua_getSiblingDB },
	{ "printjson", lua_printJson },
	{ "tojson", lua_toJson },
	{ "time", lua_time },
	{ "showDatabases", lua_showDatabases },
	{ "showCollections", lua_showCollections },
	{ "getDatabaseNames", lua_getDatabaseNames },
	{ "exit", lua_exit },
	{ "cTest", lua_cTest },
	{ NULL, NULL }
};

int LuaManager::lua_help(lua_State*) {
	printf("Functions:\n");
	printf("\tdb.help()\n");
	printf("\tdb.mycoll.help()\n");
	for (int i = 0; _bindFuncs[i].name; ++i) {
		if (_bindFuncs[i].name[0] != '_' && strcmp(_bindFuncs[i].name, "help")) {
			printf("\t%s()\n", _bindFuncs[i].name);
		}
	}
	return 0;
}

int LuaManager::lua_use(lua_State *L) {
	_pInstance->useDatabase(lua_tostring(L, 1));
	return 0;
}

int LuaManager::lua_getSiblingDB(lua_State *L) {
	LuaDB::lua_push(L, _pInstance->getDatabase(lua_tostring(L, 1)));
	return 1;
}

int LuaManager::lua_printJson(lua_State *L) {
	bool pretty = lua_gettop(L) > 1 ? lua_toboolean(L, 2) : false;
	String json;
	LuaParserUtil::luaToJson(L, json, 1, pretty);
	printf("%s\n", json.c_str());
	return 0;
}

int LuaManager::lua_toJson(lua_State *L) {
	bool pretty = lua_gettop(L) > 1 ? lua_toboolean(L, 2) : false;
	String json;
	LuaParserUtil::luaToJson(L, json, 1, pretty);
	lua_pushlstring(L, json.c_str(), json.size());
	return 1;
}

int LuaManager::lua_time(lua_State *L) {
	lua_pushinteger(L, Date::getCurrentTimeMillis());
	return 1;
}

int LuaManager::lua_exit(lua_State *L) {
	_pInstance->_isRunning = false;
	return 0;
}

int LuaManager::lua_showDatabases(lua_State *L) {
	std::vector<String> names;
	bson_error_t error;
	if (!_pInstance->_pMongo->getDatabaseNames(names, &error)) {
		luaL_error(L, "Show databases failure: %s\n", error.message);
	}
	for (int i = 0; i < names.size(); ++i) {
		printf("%s\n", names[i].c_str());
	}
	return 0;
}

int LuaManager::lua_getDatabaseNames(lua_State *L) {
	std::vector<String> names;
	bson_error_t error;
	if (!_pInstance->_pMongo->getDatabaseNames(names, &error)) {
		luaL_error(L, "Get database names failure: %s\n", error.message);
	}
	lua_createtable(L, (int)names.size(), 0);
	for (int i = 0; i < names.size(); ++i) {
		lua_pushlstring(L, names[i].c_str(), names[i].size());
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

int LuaManager::lua_showCollections(lua_State *L) {
	_pInstance->_pDB->showCollections(L);
	return 0;
}

int LuaManager::lua_cTest(lua_State *L) {
	//time_t t1;
	//int size = 100000000;
	////size = 1;

	//const uint8_t raw[5] = { 'a', 'b','c',1,5 };
	//bson_oid_t oid;
	//bson_t *scope = BCON_NEW("name", "myScope");
	//bson_oid_init(&oid, NULL);
	//bson_t *bson = BCON_NEW(
	//	"double", BCON_DOUBLE(123.4),
	//	"utf8", "this is my string",
	//	"document", BCON_DOCUMENT(scope),
	//	"array", "[", "a", BCON_BOOL(false), BCON_DOUBLE(5), "]",
	//	"binary", BCON_BIN(BSON_SUBTYPE_FUNCTION, raw, sizeof raw),
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
	//	"minkey", BCON_MINKEY
	//);
	//bson_iter_t it;
	//bson_iter_init(&it, bson);
	//bson_iter_next(&it);

	//t1 = Date::getCurrentTimeMillis();
	//for (int i = 0; i < size; ++i) {
	//	lua_createtable(L, 1, 0);
	//	lua_pushinteger(L, 666);
	//	lua_pushinteger(L, 1);	
	//	lua_rawset(L, -3);
	//	lua_pop(L, 1);
	//}
	//printf("1 %lldms\n", t1 = Date::getCurrentTimeMillis() - t1);

	//t1 = Date::getCurrentTimeMillis();
	//for (int i = 0; i < size; ++i) {
	//	lua_createtable(L, 1, 0);
	//	lua_pushinteger(L, 666);
	//	lua_rawseti(L, -2, 1);
	//	lua_pop(L, 1);
	//}
	//printf("2 %lldms\n", t1 = Date::getCurrentTimeMillis() - t1);

	//lua_createtable(L, 1, 0);
	//lua_pushinteger(L, 666);
	//lua_rawseti(L, -2, 1);

	//t1 = Date::getCurrentTimeMillis();
	//for (int i = 0; i < size; ++i) {
	//	lua_pushinteger(L, 1);
	//	lua_rawget(L, -2);
	//	lua_pop(L, 1);
	//}
	//printf("3 %lldms\n", t1 = Date::getCurrentTimeMillis() - t1);

	//t1 = Date::getCurrentTimeMillis();
	//for (int i = 0; i < size; ++i) {
	//	lua_rawgeti(L, -1, 1);
	//	lua_pop(L, 1);
	//}
	//printf("4 %lldms\n", t1 = Date::getCurrentTimeMillis() - t1);


	//t1 = Date::getCurrentTimeMillis();
	//for (int i = 0; i < size; ++i) {
	//	*(int64_t*)lua_newuserdata(L, sizeof(int64_t)) = 666;
	//	lua_pop(L, 1);
	//}
	//printf("4 %lldms\n", t1 = Date::getCurrentTimeMillis() - t1);

	//t1 = Date::getCurrentTimeMillis();
	//for (int i = 0; i < size; ++i) {
	//	*(int64_t*)lua_newuserdata(L, sizeof(int64_t)) = 666LL;
	//	lua_pop(L, 1);
	//}
	//printf("5 %lldms\n", t1 = Date::getCurrentTimeMillis() - t1);

	//t1 = Date::getCurrentTimeMillis();
	//for (int i = 0; i < size; ++i) {
	//	memcpy(lua_newuserdata(L, sizeof("lalalalalmystring")), "lalalalalmystring", sizeof("lalalalalmystring"));
	//	lua_pop(L, 1);
	//}
	//printf("6 %lldms\n", t1 = Date::getCurrentTimeMillis() - t1);

	int *p = new int(666);
	lua_pushlightuserdata(L, p);
	return 1;
}

