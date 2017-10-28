#pragma once
#include "LuaDB.h"

class MongoManager;

class LuaManager {
public:
	static LuaManager* getInstance();
	static void destroyInstance();

	MongoManager* getMongo() const;

	void useDatabase(const String name);

	const bool isRunning();
	void setIsRunning(bool isRunning);

	void loadFile(const String &path);
	void loadString(const String &str);

private:
	static LuaManager *_pInstance;

	std::unordered_map<String, LuaDB*> _databases;
	MongoManager *_pMongo;
	LuaDB *_pDB;
	bool _isRunning;
	lua_State *_L;

	LuaManager();
	~LuaManager();

	LuaDB* getDatabase(const String name);
	lua_State* createNewState();
	void printError(lua_State *L);
	void doLoadString(lua_State *L, const String &str);

	static const luaL_reg _bindFuncs[];
	
	static int lua_help(lua_State *L);
	static int lua_use(lua_State *L);
	static int lua_printJson(lua_State *L);
	static int lua_toJson(lua_State *L);
	static int lua_time(lua_State *L);
	static int lua_exit(lua_State *L);
	static int lua_getSiblingDB(lua_State *L);
	static int lua_showDatabases(lua_State *L);
	static int lua_showCollections(lua_State *L);
	static int lua_getDatabaseNames(lua_State *L);

	static int lua_cTest(lua_State *L);

};
