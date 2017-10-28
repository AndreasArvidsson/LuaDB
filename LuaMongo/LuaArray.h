#pragma once

#include "Lua.hpp"

class LuaArray {
public:
	static void lua_push(lua_State *L);
	
	LuaArray();
	~LuaArray();

private:
	static LuaArray* lua_get(lua_State *L, int index);
	
	static const luaL_reg _constructorBindFuncs[];
	static int lua_create(lua_State *L);
	static int lua_constructorToString(lua_State *L);

	static const luaL_reg _tableBindFuncs[];
	static int lua_tableToString(lua_State *L);
	static int lua_delete(lua_State *L);

};

