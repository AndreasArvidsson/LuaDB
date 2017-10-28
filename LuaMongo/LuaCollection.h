#pragma once
#include "Lua.hpp"
#include "String.h"

class MongoCollection;

class LuaCollection {
public:
	static void lua_push(lua_State *L, LuaCollection *collection);

	LuaCollection(MongoCollection *pMongoCollection);
	~LuaCollection();

private:
	MongoCollection *_pMongoCollection;

	static LuaCollection* lua_get(lua_State *L, const int index);

	static const luaL_reg _bindFuncs[];
	static int lua_toString(lua_State *L);
	static int lua_help(lua_State *L);
	static int lua_getName(lua_State *L);
	static int lua_getFullName(lua_State *L);
	static int lua_renameCollection(lua_State *L);
	static int lua_find(lua_State *L);
	static int lua_findOne(lua_State *L);
	static int lua_aggregate(lua_State *L);
	static int lua_count(lua_State *L);
	static int lua_distinct(lua_State *L);
	static int lua_insert(lua_State *L);
	static int lua_insertMany(lua_State *L);
	static int lua_save(lua_State *L);
	static int lua_update(lua_State *L);
	static int lua_remove(lua_State *L);
	static int lua_runCommand(lua_State *L);
	static int lua_drop(lua_State *L);
	static int lua_createIndex(lua_State *L);
	static int lua_dropIndex(lua_State *L);

	static int lua_createTestData(lua_State *L);
};


