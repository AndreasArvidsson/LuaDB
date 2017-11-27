#pragma once
#include <unordered_map>
#include "LuaCollection.h"

class MongoDB;

class LuaDB {
public:
	static void lua_push(lua_State *L, LuaDB *pDB);
	static void lua_registerDB(lua_State *L, LuaDB *pDB);

	LuaDB(MongoDB *pMongoDB);
	~LuaDB();

	LuaCollection* getCollection(const String name);
	void showCollections(lua_State *L);

private:
	MongoDB *_pMongoDB;
	std::unordered_map<String, LuaCollection*> _collections;

	static LuaDB* lua_get(lua_State *L, int index);

	static const luaL_reg _bindFuncs[];
	static int lua_index(lua_State *L);
	static int lua_help(lua_State *L);
	static int lua_getName(lua_State *L);
	static int lua_getCollection(lua_State *L);
	static int lua_hasCollection(lua_State *L);
	static int lua_getCollectionNames(lua_State *L);
	static int lua_showCollections(lua_State *L);
	static int lua_createCollection(lua_State *L);
	static int lua_runCommand(lua_State *L);	
	static int lua_drop(lua_State *L);
};

