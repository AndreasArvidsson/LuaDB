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

using std::string;

/*************************************
*************** STATIC ***************
**************************************/

std::unique_ptr<LuaManager> LuaManager::_pInstance;

LuaManager* LuaManager::getInstance() {
    if (!_pInstance) {
        _pInstance = std::make_unique<LuaManager>();
    }
    return _pInstance.get();
}

void LuaManager::destroyInstance() {
    _pInstance = nullptr;
}

/*************************************
*************** PUBLIC ***************
**************************************/

LuaManager::LuaManager() {
    _pMongo = std::make_unique<MongoManager>();
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
    _pMongo = nullptr;
}

MongoManager* LuaManager::getMongo() const {
    return _pMongo.get();
}

void LuaManager::useDatabase(const string& name) {
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

void LuaManager::loadFile(const string& path) {
    const File file(path);
    if (file.exists()) {
        string str;
        if (file.getData(str) && str.size()) {
            loadString(str);
        }
        else {
            printf("Error: Can't read from file '%s'\n", path.c_str());
        }
    }
    else {
        printf("Error: File '%s' doesn't exist\n", path.c_str());
    }
}

void LuaManager::loadString(const string& str) {
    if (!_L) {
        _L = createNewState();
    }
    doLoadString(_L, str);
}


/*************************************
*************** PRIVATE **************
**************************************/

lua_State* LuaManager::createNewState() {
    //Create new lua state
    lua_State* L = luaL_newstate();

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

void LuaManager::doLoadString(lua_State* L, const string& str) {
    if (luaL_loadstring(L, str.c_str())) {
        printError(L);
        return;
    }
    if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
        printError(L);
    }
}

void LuaManager::printError(lua_State* L) {
    puts(lua_tostring(L, -1));
    lua_pop(L, 1);
}

LuaDB* LuaManager::getDatabase(const string& name) {
    const auto found = _databases.find(name);
    //Already have this DB.
    if (found != _databases.end()) {
        return found->second.get();
    }
    //Create new DB
    MongoDB* pMongoDB = _pMongo->getDatabase(name);
    _databases[name] = std::make_unique<LuaDB>(pMongoDB);
    return _databases[name].get();
}

/*************************************
************** LUA BIND **************
**************************************/

const luaL_reg LuaManager::_bindFuncs[] = {
    { "help", lua_help },
    { "use", lua_use },
    { "getSiblingDB", lua_getSiblingDB },
    { "print", lua_print },
    { "printjson", lua_printJson },
    { "tojson", lua_toJson },
    { "time", lua_time },
    { "showDatabases", lua_showDatabases },
    { "showCollections", lua_showCollections },
    { "getDatabaseNames", lua_getDatabaseNames },
    { "exit", lua_exit },
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

int LuaManager::lua_use(lua_State* L) {
    _pInstance->useDatabase(lua_tostring(L, 1));
    return 0;
}

int LuaManager::lua_getSiblingDB(lua_State* L) {
    LuaDB::lua_push(L, _pInstance->getDatabase(lua_tostring(L, 1)));
    return 1;
}

int LuaManager::lua_print(lua_State* L) {
    const int argc = lua_gettop(L);
    for (int i = 1; i <= argc; ++i) {
        LuaParserUtil::printLua(L, i);
        if (i < argc) {
            printf(" ");
        }
    }
    printf("\n");
    return 0;
}

int LuaManager::lua_printJson(lua_State* L) {
    bool pretty = lua_gettop(L) > 1 ? lua_toboolean(L, 2) : false;
    string json;
    LuaParserUtil::luaToJson(L, json, 1, pretty);
    printf("%s\n", json.c_str());
    return 0;
}

int LuaManager::lua_toJson(lua_State* L) {
    bool pretty = lua_gettop(L) > 1 ? lua_toboolean(L, 2) : false;
    string json;
    LuaParserUtil::luaToJson(L, json, 1, pretty);
    lua_pushlstring(L, json.c_str(), json.size());
    return 1;
}

int LuaManager::lua_time(lua_State* L) {
    lua_pushinteger(L, Date::getCurrentTimeMillis());
    return 1;
}

int LuaManager::lua_exit(lua_State* L) {
    _pInstance->_isRunning = false;
    return 0;
}

int LuaManager::lua_showDatabases(lua_State* L) {
    std::vector<string> names;
    bson_error_t error;
    if (!_pInstance->_pMongo->getDatabaseNames(names, &error)) {
        luaL_error(L, "Show databases failure: %s\n", error.message);
    }
    for (int i = 0; i < names.size(); ++i) {
        printf("%s\n", names[i].c_str());
    }
    return 0;
}

int LuaManager::lua_getDatabaseNames(lua_State* L) {
    std::vector<string> names;
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

int LuaManager::lua_showCollections(lua_State* L) {
    _pInstance->_pDB->showCollections(L);
    return 0;
}