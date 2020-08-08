
#include "LuaCursor.h"
#include "LuaParserUtil.h"
#include <cstdio> /* sprintf */

#define CURSOR_TABLE_NAME "MongoCursor"

/*************************************
*************** STATIC ***************
**************************************/

void LuaCursor::lua_push(lua_State* L, MongoCollection* pMongoCollection, mongoc_cursor_t* pMongoCursor) {
    LuaCursor* pCursor = new LuaCursor(pMongoCollection, pMongoCursor);
    *(LuaCursor**)(lua_newuserdata(L, sizeof(LuaCursor*))) = pCursor;
    if (luaL_newmetatable(L, CURSOR_TABLE_NAME)) {
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        lua_setStr(L, "Protected", "__metatable");
        lua_setFuncs(L, _bindFuncs);
    }
    lua_setmetatable(L, -2);
}

/*************************************
*************** PUBLIC ***************
**************************************/

LuaCursor::LuaCursor(MongoCollection* pMongoCollection, mongoc_cursor_t* pMongoCursor) {
    _pMongoCollection = pMongoCollection;
    _pMongoCursor = pMongoCursor;
    _limit = _index = 0;
}

LuaCursor::~LuaCursor() {
    mongoc_cursor_destroy(_pMongoCursor);
}

/*************************************
*************** PRIVATE **************
**************************************/

LuaCursor* LuaCursor::lua_get(lua_State* L, const int index) {
    return *reinterpret_cast<LuaCursor**>(luaL_checkudata(L, index, CURSOR_TABLE_NAME));
}

/*************************************
************** LUA BIND **************
**************************************/

const luaL_reg LuaCursor::_bindFuncs[] = {
    { "__tostring", lua_toString },
    { "__gc", lua_delete },
    { "next", lua_next },
    { "forEach", lua_forEach },
    { "iterator", lua_iterator },
    { "toArray", lua_toArray },
    { "limit", lua_limit },
    { NULL, NULL }
};

int LuaCursor::lua_toString(lua_State* L) {
    //MongoCLI ex: 'DBQuery: gss.test1 -> { "a" : 2 }'
    LuaCursor* pCursor = lua_get(L, 1);
    char buf[128];
    size_t length = snprintf(buf, 128, "DBQuery: %s", pCursor->_pMongoCollection->getFullName().c_str());
    lua_pushlstring(L, buf, length);
    return 1;
}

int LuaCursor::lua_delete(lua_State* L) {
    delete lua_get(L, 1);
    return 0;
}

int LuaCursor::lua_next(lua_State* L) {
    LuaCursor* pCursor = lua_get(L, 1);
    if (pCursor->_limit == 0 || pCursor->_index < pCursor->_limit) {
        const bson_t* pReply;
        //Has next. Return it.
        if (mongoc_cursor_next(pCursor->_pMongoCursor, &pReply)) {
            LuaParserUtil::bsonToLua(pReply, L);
            ++pCursor->_index;
            return 1;
        }
        //No next. Check for errors.
        bson_error_t error;
        if (mongoc_cursor_error(pCursor->_pMongoCursor, &error)) {
            luaL_error(L, "Cursor Failure: %s\n", error.message);
        }
    }
    return 0;
}

int LuaCursor::lua_forEach(lua_State* L) {
    LuaCursor* pCursor = lua_get(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    const bson_t* pReply;
    while ((pCursor->_limit == 0 || pCursor->_index < pCursor->_limit) && mongoc_cursor_next(pCursor->_pMongoCursor, &pReply)) {
        lua_pushvalue(L, 2);
        LuaParserUtil::bsonToLua(pReply, L);
        lua_pcall(L, 1, 0, 0);
        ++pCursor->_index;
    }

    //No next. Check for errors.
    bson_error_t error;
    if (mongoc_cursor_error(pCursor->_pMongoCursor, &error)) {
        luaL_error(L, "Cursor Failure: %s\n", error.message);
    }

    return 0;
}

int LuaCursor::lua_iterator(lua_State* L) {
    luaL_checkudata(L, 1, CURSOR_TABLE_NAME);
    lua_pushcclosure(L, lua_iteratorCallback, 1);
    return 1;
}

int LuaCursor::lua_iteratorCallback(lua_State* L) {
    LuaCursor* pCursor = lua_get(L, lua_upvalueindex(1));
    const bson_t* pReply;
    if ((pCursor->_limit == 0 || pCursor->_index < pCursor->_limit) && mongoc_cursor_next(pCursor->_pMongoCursor, &pReply)) {
        LuaParserUtil::bsonToLua(pReply, L);
        ++pCursor->_index;
        return 1;
    }
    return 0;
}

int LuaCursor::lua_toArray(lua_State* L) {
    LuaCursor* pCursor = lua_get(L, 1);
    lua_newtable(L);
    const bson_t* pReply;
    int index = 0;
    while ((pCursor->_limit == 0 || pCursor->_index < pCursor->_limit) && mongoc_cursor_next(pCursor->_pMongoCursor, &pReply)) {
        lua_pushinteger(L, ++index);
        LuaParserUtil::bsonToLua(pReply, L);
        lua_settable(L, -3);
        ++pCursor->_index;
    }
    return 1;
}

int LuaCursor::lua_limit(lua_State* L) {
    LuaCursor* pCursor = lua_get(L, 1);
    pCursor->_limit = lua_tointeger(L, 2);
    lua_pushvalue(L, 1);
    return 1;
}