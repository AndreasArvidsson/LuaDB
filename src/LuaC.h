#pragma once
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luajit.h"

#if LUA_VERSION_NUM == 501
#define lua_setFuncs(L, list) luaL_register(L, NULL, list)
#define luaL_setmetatable(L, name) luaL_getmetatable(L, name); lua_setmetatable(L, -2)
#define lua_pushglobaltable(L) lua_pushvalue(L, LUA_GLOBALSINDEX)
#define lua_rawlen(L, index) lua_objlen(L, index) 
LUALIB_API void *luaL_testudata(lua_State *L, int ud, const char *tname);
#elif LUA_VERSION_NUM == 502
#define lua_setFuncs(L, list) luaL_setfuncs(L, list, 0)
#endif

#ifndef lua_callFunc
#define lua_callFunc(L, name) lua_pushcfunction(L, func); lua_call(L, 0, 0)
#endif

#ifndef lua_setFunc
#define lua_setFunc(L, func, name) lua_pushcfunction(L, func); lua_setfield(L, -2, name)
#endif

#ifndef lua_setInt
#define lua_setInt(L, val, name) lua_pushinteger(L, val); lua_setfield(L, -2, name)
#endif

#ifndef lua_setBool
#define lua_setBool(L, val, name) lua_pushboolean(L, val); lua_setfield(L, -2, name)
#endif

#ifndef lua_setStr
#define lua_setStr(L, val, name) lua_pushstring(L, val); lua_setfield(L, -2, name)
#endif

#ifndef lua_getTable
#define lua_getTable(L, key) lua_pushstring(L, key); lua_gettable(L, -2);
#endif

int luaL_testarray(lua_State *L, const int index);
int luaL_testtable(lua_State *L, const int index, const char *tname);
void luaL_validatetable(lua_State *L, const int index, const char *tname);
void luaL_validatefunction(lua_State *L, const int index);
const char* luaL_validatestring(lua_State *L, const int index);
const char* luaL_validatelstring(lua_State *L, const int index, size_t *lengthOut);
const lua_Number luaL_validatenumber(lua_State *L, const int index);
const int luaL_validatebool(lua_State *L, const int index);

void lua_printStack(lua_State *L);