#include "LuaArray.h"

#define ARRAY_CONSTRUCTOR_NAME "Array constructor"
#define ARRAY_TABLE_NAME "Array"

/*************************************
*************** STATIC ***************
**************************************/

void LuaArray::lua_push(lua_State *L) {
	
	luaL_newmetatable(L, ARRAY_CONSTRUCTOR_NAME);
	lua_setFuncs(L, _constructorBindFuncs);
	lua_newuserdata(L, 1);
	luaL_setmetatable(L, ARRAY_CONSTRUCTOR_NAME);
	lua_setglobal(L, ARRAY_TABLE_NAME);

	//LuaArray *pArray = new LuaArray();

	//*(LuaArray**)(lua_newuserdata(L, sizeof(LuaArray*))) = pArray;
	//if (luaL_newmetatable(L, ARRAY_TABLE_NAME)) {
	//	lua_pushvalue(L, -1);
	//	lua_setfield(L, -2, "__index");
	//	lua_setFuncs(L, _bindFuncs);
	//}
	//lua_setmetatable(L, -2);
	//lua_setglobal(L, ARRAY_TABLE_NAME);
}

/*************************************
*************** PUBLIC ***************
**************************************/

LuaArray::LuaArray() {
}

LuaArray::~LuaArray() {
}

/*************************************
************** LUA BIND **************
**************************************/

LuaArray* LuaArray::lua_get(lua_State *L, const int index) {
	return *reinterpret_cast<LuaArray**>(luaL_checkudata(L, index, ARRAY_TABLE_NAME));
}

const luaL_reg LuaArray::_constructorBindFuncs[] = {
	{ "__call", lua_create },
	{ "__tostring", lua_constructorToString },
	{ NULL, NULL }
};

int LuaArray::lua_create(lua_State *L) {
	//*(LuaArray**)(lua_newuserdata(L, sizeof(LuaArray*))) = new LuaArray();

	lua_newtable(L);

	if (luaL_newmetatable(L, ARRAY_TABLE_NAME)) {
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_setFuncs(L, _tableBindFuncs);
	}
	lua_setmetatable(L, -2);
	return 1;
}

int LuaArray::lua_constructorToString(lua_State *L) {
	lua_pushstring(L, ARRAY_CONSTRUCTOR_NAME);
	return 1;
}

const luaL_reg LuaArray::_tableBindFuncs[] = {
	{ "__tostring", lua_tableToString },
	{ "__gc", lua_delete },
	{ NULL, NULL }
};

int LuaArray::lua_tableToString(lua_State *L) {
	lua_pushstring(L, ARRAY_TABLE_NAME);
	return 1;
}

int LuaArray::lua_delete(lua_State *L) {
	LuaArray *pArray = lua_get(L, 1);
	delete pArray;
	return 0;
}

