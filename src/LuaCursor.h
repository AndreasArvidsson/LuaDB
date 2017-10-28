#pragma once
#include "Lua.hpp"
#include "MongoCollection.h"

class LuaCursor {
public:
	static void lua_push(lua_State *L, LuaCursor *pCursor);

	LuaCursor(MongoCollection *pMongoCollection, mongoc_cursor_t *pMongoCursor);
	~LuaCursor();

private:
	MongoCollection *_pMongoCollection;
	mongoc_cursor_t *_pMongoCursor;
	size_t _limit, _index;

	static LuaCursor* lua_get(lua_State *L, int index);
	static const luaL_reg _bindFuncs[];
	static int lua_toString(lua_State *L);
	static int lua_delete(lua_State *L);
	static int lua_next(lua_State *L);
	static int lua_forEach(lua_State *L);
	static int lua_iteratorCallback(lua_State *L);
	static int lua_iterator(lua_State *L);
	static int lua_toArray(lua_State *L);
	static int lua_limit(lua_State *L);	
};