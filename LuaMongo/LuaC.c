#include "LuaC.h"

#if LUA_VERSION_NUM == 501

LUALIB_API void* luaL_testudata(lua_State *L, int ud, const char *tname) {
	void *p = lua_touserdata(L, ud);
	if (p != NULL) {  /* value is a userdata? */
		if (lua_getmetatable(L, ud)) {  /* does it have a metatable? */
			luaL_getmetatable(L, tname);  /* get correct metatable */
			if (!lua_rawequal(L, -1, -2))  /* not the same? */
				p = NULL;  /* value is a userdata with wrong metatable */
			lua_pop(L, 2);  /* remove both metatables */
			return p;
		}
	}
	return NULL;  /* value is not a userdata with a metatable */
}

#endif

int luaL_testarray(lua_State *L, const int index) {
	size_t len = lua_rawlen(L, index);
	if (len > 0) {
		lua_pushinteger(L, len);
		if (!lua_next(L, index < 0 ? index -1 : index)) {
			return 1;
		}
		else {
			lua_pop(L, 2);
		}
	}
	return luaL_testtable(L, index, "Array");
}

int luaL_testtable(lua_State *L, const int index, const char *tname) {
	if (!lua_istable(L, index)) {
		return 0;
	}
	int tmp = 0;
	if (lua_getmetatable(L, index)) {
		luaL_getmetatable(L, tname);
		if (lua_rawequal(L, -1, -2)) {
			tmp = 1;
		}
		lua_pop(L, 2);
	}
	return tmp;
}

void luaL_validatetable(lua_State *L, const int index, const char *tname) {
	if (!luaL_testtable(L, index, tname)) {
		luaL_typerror(L, index, tname);
	}
}

int validate(lua_State *L, const int index, const int expectedType) {
	if (lua_type(L, index) != expectedType) {
		luaL_typerror(L, index, lua_typename(L, expectedType));
	}
	return 0;
}

void luaL_validatefunction(lua_State *L, const int index) {
	validate(L, index, LUA_TFUNCTION);
}

const char* luaL_validatestring(lua_State *L, const int index) {
	validate(L, index, LUA_TSTRING);
	return lua_tostring(L, index);
}

const char* luaL_validatelstring(lua_State *L, const int index, size_t *lengthOut) {
	validate(L, index, LUA_TSTRING);
	return lua_tolstring(L, index, lengthOut);
}

const lua_Number luaL_validatenumber(lua_State *L, const int index) {
	validate(L, index, LUA_TNUMBER);
	return lua_tonumber(L, index);
}

const int luaL_validatebool(lua_State *L, const int index) {
	validate(L, index, LUA_TBOOLEAN);
	return lua_toboolean(L, index);
}

void lua_printStack(lua_State *L) {
	printf("-------------------\n");
	int size = lua_gettop(L);
	for (int i = size; i > 0; --i) {
		int type = lua_type(L, i);
		printf("#%d (%d) %s", i, i - size - 1, lua_typename(L, type));
		switch (type) {
		case LUA_TBOOLEAN:
			printf("\t%s", lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			printf("\t%f", lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			printf("\t%s", lua_tostring(L, i));
			break;
		}
		printf("\n");
	}
	printf("-------------------\n");
}



/* convert a stack index to positive */
//#define abs_index(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
