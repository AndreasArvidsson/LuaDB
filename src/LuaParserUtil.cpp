#include "LuaParserUtil.h"
#include <stdlib.h>
#include "LuaBsonTypes.h"

using std::string;

/*************************************
************* LUA TO BSON ************
**************************************/

void LuaParserUtil::luaToBson(lua_State* L, bson_t* bsonOut, const int index) {
    if (luaL_getmetafield(L, index, "__bson")) {
        const int bsonType = (int)lua_tointeger(L, -1);
        //Pop metafield
        lua_pop(L, 1);
        switch (bsonType) {
        case BSON_TYPE_DOCUMENT: {
            luaDocumentToBson(L, bsonOut, index);
            break;
        }
        case BSON_TYPE_ARRAY: {
            luaArrayToBson(L, bsonOut, index);
            break;
        }
        default:
            luaL_error(L, "Unknown bson type '%d'", bsonType);
        }
    }
    else {
        switch (lua_type(L, index)) {
        case LUA_TTABLE:
            if (luaL_testarray(L, index)) {
                luaArrayToBson(L, bsonOut, index);
            }
            else {
                luaTableToBson(L, bsonOut, index);
            }
            break;
        default:
            luaL_typerror(L, index, lua_typename(L, LUA_TTABLE));
        }
    }
}

void LuaParserUtil::luaTableToBson(lua_State* L, bson_t* pBsonOut, int index) {
    size_t keyLength;
    const char* keyStr;
    if (index < 0) {
        --index;
    }
    for (lua_pushnil(L); lua_next(L, index); lua_pop(L, 1)) {
        keyStr = lua_tolstring(L, -2, &keyLength);
        luaObjectToBson(L, pBsonOut, -1, keyStr, (int)keyLength);
    }
}

void LuaParserUtil::luaArrayToBson(lua_State* L, bson_t* pBsonOut, int index) {
    char buf[16];
    uint32_t arrayIndex = 0;
    size_t keyLength;
    const char* keyStr;
    if (index < 0) {
        --index;
    }
    for (lua_pushnil(L); lua_next(L, index); lua_pop(L, 1)) {
        keyLength = bson_uint32_to_string(arrayIndex++, &keyStr, buf, sizeof buf);
        luaObjectToBson(L, pBsonOut, -1, keyStr, (int)keyLength);
    }
}

void LuaParserUtil::luaDocumentToBson(lua_State* L, bson_t* pBsonOut, int index) {
    const char* keyStr;
    size_t keyLength;
    if (index < 0) {
        --index;
    }
    for (lua_pushnil(L); LuaBsonTypes::lua_documentNext(L, index); lua_pop(L, 2)) {
        keyStr = lua_tolstring(L, -2, &keyLength);
        luaObjectToBson(L, pBsonOut, -1, keyStr, (int)keyLength);
    }
}

void LuaParserUtil::luaObjectToBson(lua_State* L, bson_t* pBsonOut, const int index, const char* keyStr, const int keyLength) {
    //Bson
    if (luaL_getmetafield(L, index, "__bson")) {
        const int bsonType = (int)lua_tointeger(L, -1);
        //Pop metafield
        lua_pop(L, 1);
        switch (bsonType) {
        case BSON_TYPE_DOCUMENT: {
            bson_t child;
            bson_append_document_begin(pBsonOut, keyStr, keyLength, &child);
            luaDocumentToBson(L, &child, index);
            bson_append_document_end(pBsonOut, &child);
            break;
        }
        case BSON_TYPE_ARRAY: {
            bson_t child;
            bson_append_array_begin(pBsonOut, keyStr, keyLength, &child);
            luaArrayToBson(L, &child, index);
            bson_append_array_end(pBsonOut, &child);
            break;
        }
        case BSON_TYPE_BINARY: {
            uint32_t length;
            bson_subtype_t subType;
            const uint8_t* pBinary;
            LuaBsonTypes::lua_readBinary(L, index, &length, &subType, &pBinary);
            bson_append_binary(pBsonOut, keyStr, keyLength, subType, pBinary, length);
            break;
        }
        case BSON_TYPE_OID:
            bson_append_oid(pBsonOut, keyStr, keyLength, LuaBsonTypes::lua_readObjectId(L, index));
            break;
        case BSON_TYPE_DATE_TIME:
            bson_append_date_time(pBsonOut, keyStr, keyLength, LuaBsonTypes::lua_readIsoDate(L, index));
            break;
        case BSON_TYPE_NULL:
            bson_append_null(pBsonOut, keyStr, keyLength);
            break;
        case BSON_TYPE_REGEX: {
            const char* pRegexp, * pOptions;
            LuaBsonTypes::lua_readRegexp(L, index, &pRegexp, &pOptions);
            bson_append_regex(pBsonOut, keyStr, keyLength, pRegexp, pOptions);
            break;
        }
        case BSON_TYPE_DBPOINTER: {
            const char* pCollection;
            const bson_oid_t* pOid;
            LuaBsonTypes::lua_readDbPointer(L, index, &pCollection, &pOid);
            bson_append_dbpointer(pBsonOut, keyStr, keyLength, pCollection, pOid);
            break;
        }
        case BSON_TYPE_CODE:
            bson_append_code(pBsonOut, keyStr, keyLength, LuaBsonTypes::lua_readCode(L, index));
            break;
        case BSON_TYPE_SYMBOL: {
            const char* pSymbol;
            uint32_t length;
            LuaBsonTypes::lua_readSymbol(L, index, &pSymbol, &length);
            bson_append_symbol(pBsonOut, keyStr, keyLength, pSymbol, length);
            break;
        }
        case BSON_TYPE_CODEWSCOPE: {
            lua_pushvalue(L, index);
            lua_pushlstring(L, "code", 4);
            lua_rawget(L, -2);
            const char* pCode = lua_tostring(L, -1);
            lua_pushlstring(L, "scope", 5);
            lua_rawget(L, -3);
            bson_t* pScope = bson_new();
            luaToBson(L, pScope, -1);
            bson_append_code_with_scope(pBsonOut, keyStr, keyLength, pCode, pScope);
            bson_destroy(pScope);
            lua_pop(L, 3);

            //const char *pCode;
            //const bson_t *pScope;
            //LuaBsonTypes::lua_readCodeWithScope(L, index, &pCode, &pScope);
            //bson_append_code_with_scope(pBsonOut, keyStr, keyLength, pCode, pScope);
            break;
        }
        case BSON_TYPE_INT32:
            bson_append_int32(pBsonOut, keyStr, keyLength, LuaBsonTypes::lua_readNumberInt(L, index));
            break;
        case BSON_TYPE_TIMESTAMP: {
            uint32_t timestamp, increment;
            LuaBsonTypes::lua_readTimestamp(L, index, &timestamp, &increment);
            bson_append_timestamp(pBsonOut, keyStr, keyLength, timestamp, increment);
            break;
        }
        case BSON_TYPE_INT64:
            bson_append_int64(pBsonOut, keyStr, keyLength, LuaBsonTypes::lua_readNumberLong(L, index));
            break;
        case BSON_TYPE_DECIMAL128:
            bson_append_decimal128(pBsonOut, keyStr, keyLength, LuaBsonTypes::lua_readNumberDecimal(L, index));
            break;
        case BSON_TYPE_MAXKEY:
            bson_append_maxkey(pBsonOut, keyStr, keyLength);
            break;
        case BSON_TYPE_MINKEY:
            bson_append_minkey(pBsonOut, keyStr, keyLength);
            break;
        default:
            luaL_error(L, "Unknown bson type '%d' for field '%s'", bsonType, keyStr);
        }
    }
    //Other
    else {
        switch (lua_type(L, -1)) {
        case LUA_TNONE:
        case LUA_TNIL:
            bson_append_undefined(pBsonOut, keyStr, keyLength);
            break;
        case LUA_TBOOLEAN:
            bson_append_bool(pBsonOut, keyStr, keyLength, lua_toboolean(L, -1));
            break;
        case LUA_TNUMBER:
            bson_append_double(pBsonOut, keyStr, keyLength, lua_tonumber(L, -1));
            break;
        case LUA_TSTRING: {
            size_t valueLength;
            const char* valueStr = lua_tolstring(L, -1, &valueLength);
            bson_append_utf8(pBsonOut, keyStr, keyLength, valueStr, (int)valueLength);
            break;
        }
        case LUA_TTABLE:
            if (luaL_testarray(L, -1)) {
                bson_t child;
                bson_append_array_begin(pBsonOut, keyStr, keyLength, &child);
                luaArrayToBson(L, &child, -1);
                bson_append_array_end(pBsonOut, &child);
            }
            else {
                bson_t child;
                bson_append_document_begin(pBsonOut, keyStr, keyLength, &child);
                luaTableToBson(L, &child, -1);
                bson_append_document_end(pBsonOut, &child);
            }
            break;
        default:
            luaL_error(L, "Unsupported LUA type '%s' for field '%s'", lua_typename(L, lua_type(L, -1)), keyStr);
        }
    }
}

/*************************************
************ LUA TO JSON *************
**************************************/

void LuaParserUtil::luaToJson(lua_State* L, string& jsonOut, const int index, const bool pretty, const int depth) {
    if (luaL_getmetafield(L, index, "__bson")) {
        int bsonType = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);
        switch (bsonType) {
        case BSON_TYPE_DOCUMENT:
            luaDocumentToJson(L, jsonOut, index, pretty, depth + 1);
            return;
        case BSON_TYPE_ARRAY:
            luaArrayToJson(L, jsonOut, index, pretty, depth + 1);
            return;
        case BSON_TYPE_WRITE_RESULT:
            luaTableToJson(L, jsonOut, index, pretty, depth + 1);
            return;
        }
        //If the object has a tostring meta method use it.
        if (luaL_callmeta(L, index, "__tostring")) {
            jsonOut += lua_tostring(L, -1);
            lua_pop(L, 1);
            return;
        }
        luaTableToJson(L, jsonOut, index, pretty, depth + 1);
    }
    else {
        switch (lua_type(L, index)) {
        case LUA_TNONE:
        case LUA_TNIL:
            jsonOut += "undefined";
            break;
        case LUA_TBOOLEAN:
            jsonOut += lua_toboolean(L, index) ? "true" : "false";
            break;
        case LUA_TNUMBER: {
            char buf[24];
            snprintf(buf, 24, "%g", lua_tonumber(L, index));
            jsonOut += buf;
            break;
        }
        case LUA_TSTRING:
            jsonOut += "\"";
            jsonOut += lua_tostring(L, index);
            jsonOut += "\"";
            break;
        case LUA_TTABLE:
            if (luaL_testarray(L, index)) {
                luaArrayToJson(L, jsonOut, index, pretty, depth + 1);
            }
            else {
                luaTableToJson(L, jsonOut, index, pretty, depth + 1);
            }
            break;
        case LUA_TFUNCTION:
            jsonOut += "function (...) { ... }";
            break;
        default:
            luaL_error(L, "Unknown LUA type '%s'", lua_typename(L, lua_type(L, -1)));
        }
    }
}

void LuaParserUtil::luaDocumentToJson(lua_State* L, string& jsonOut, int index, const bool pretty, const int depth) {
    string indent = pretty ? getIndent(depth) : "";
    jsonOut += pretty ? "{\n" : "{ ";
    bool first = true;
    if (index < 0) {
        --index;
    }
    for (lua_pushnil(L); LuaBsonTypes::lua_documentNext(L, index); lua_pop(L, 2)) {
        if (first) {
            first = false;
        }
        else if (pretty) {
            jsonOut += ",\n";
        }
        else {
            jsonOut += ", ";
        }
        jsonOut += indent;
        jsonOut += "\"";
        jsonOut += lua_tostring(L, -2);
        jsonOut += "\" : ";
        luaToJson(L, jsonOut, -1, pretty, depth);
    }
    jsonOut += pretty ? "\n" + getIndent(depth - 1) + "}" : " }";
}

void LuaParserUtil::luaTableToJson(lua_State* L, string& jsonOut, int index, const bool pretty, const int depth) {
    string indent = pretty ? getIndent(depth) : "";
    jsonOut += pretty ? "{\n" : "{ ";
    bool first = true;
    if (index < 0) {
        --index;
    }
    for (lua_pushnil(L); lua_next(L, index); lua_pop(L, 1)) {
        if (first) {
            first = false;
        }
        else if (pretty) {
            jsonOut += ",\n";
        }
        else {
            jsonOut += ", ";
        }
        jsonOut += indent;
        jsonOut += "\"";
        jsonOut += lua_tostring(L, -2);
        jsonOut += "\" : ";
        luaToJson(L, jsonOut, -1, pretty, depth);
    }
    jsonOut += pretty ? "\n" + getIndent(depth - 1) + "}" : " }";
}

void LuaParserUtil::luaArrayToJson(lua_State* L, string& jsonOut, int index, const bool pretty, const int depth) {
    string indent = pretty ? getIndent(depth) : "";
    jsonOut += pretty ? "[\n" : "[ ";
    bool first = true;
    if (index < 0) {
        --index;
    }
    for (lua_pushnil(L); lua_next(L, index); lua_pop(L, 1)) {
        if (first) {
            first = false;
        }
        else if (pretty) {
            jsonOut += ",\n";
        }
        else {
            jsonOut += ", ";
        }
        jsonOut += indent;
        luaToJson(L, jsonOut, -1, pretty, depth);
    }
    jsonOut += pretty ? "\n" + getIndent(depth - 1) + "]" : " ]";
}

const string LuaParserUtil::getIndent(int numIndents) {
    string str;
    while (numIndents--) {
        str += "    ";
    }
    return str;
}

/*************************************
************* BSON TO LUA ************
**************************************/

void LuaParserUtil::bsonToLua(const bson_t* bsonIn, lua_State* L) {
    bson_iter_t it;
    if (bson_iter_init(&it, bsonIn)) {
        bsonDocumentToLua(&it, L);
    }
}

void LuaParserUtil::bsonToLuaTable(const bson_t* bsonIn, lua_State* L) {
    bson_iter_t it;
    if (bson_iter_init(&it, bsonIn)) {
        bsonDocumentToLuaTable(&it, L);
    }
}

void LuaParserUtil::bsonSubToLua(const bson_t* bson, lua_State* L, const char* field) {
    bson_iter_t it;
    if (bson_iter_init_find(&it, bson, field)) {
        bson_iter_t childIt;
        switch (bson_iter_type(&it)) {
        case BSON_TYPE_DOCUMENT:
            if (bson_iter_recurse(&it, &childIt)) {
                bsonDocumentToLua(&childIt, L);
            }
            break;
        case BSON_TYPE_ARRAY:
            if (bson_iter_recurse(&it, &childIt)) {
                bsonArrayToLua(&childIt, L);
            }
            break;
        default:
            luaL_error(L, "Unknown bson type '%d'", bson_iter_type(&childIt));
        }
    }
}

//No keys table. slow delete
void LuaParserUtil::bsonDocumentToLua(bson_iter_t* it, lua_State* L) {
    const int size = bson_iter_documentSize(it);
    lua_createtable(L, 0, size + 2); //Main table -3
    luaL_setmetatable(L, BSON_NAME_DOCUMENT);
    lua_pushlstring(L, "__order", 7);
    lua_createtable(L, size, 0); //Order table -1

    for (int i = 1; bson_iter_next(it); ++i) {
        //Prepare values
        lua_pushlstring(L, bson_iter_key(it), bson_iter_keyLength(it));
        lua_pushinteger(L, i);
        lua_pushvalue(L, -2);
        //Set key in order table.
        lua_rawset(L, -4);
        //Set value in main table
        bsonObjectToLua(it, L);
        lua_rawset(L, -5);
    }

    //Add keys and order tables to main table.
    lua_rawset(L, -3);
    //Add next index to main table
    lua_pushlstring(L, "__nextIndex", 11);
    lua_pushinteger(L, size + 1);
    lua_rawset(L, -3);
}

//Keys table with fast delete support
//void LuaParserUtil::bsonDocumentToLua(bson_iter_t *it, lua_State *L) {
//	const int size = bson_iter_documentSize(it);
//	lua_createtable(L, 0, size + 3); //Main table -5
//	luaL_setmetatable(L, BSON_NAME_DOCUMENT);
//	lua_pushlstring(L, "__keys", 6);
//	lua_createtable(L, 0, size); //Keys table -3
//	lua_pushlstring(L, "__order", 7);
//	lua_createtable(L, size, 0); //Order table -1
//
//	for (int i = 1; bson_iter_next(it); ++i) {
//		//Prepare values
//		lua_pushlstring(L, bson_iter_key(it), bson_iter_keyLength(it));
//		lua_pushvalue(L, -1);
//		lua_pushinteger(L, i);
//		lua_pushinteger(L, i);
//		lua_pushvalue(L, -3);
//		//Set key in order table.
//		lua_rawset(L, -6);
//		//Set index in key table
//		lua_rawset(L, -6);
//		//Set value in main table
//		bsonObjectToLua(it, L);
//		lua_rawset(L, -7);
//	}
//
//	//Add keys and order tables to main table.
//	lua_rawset(L, -5);
//	lua_rawset(L, -3);
//	//Add next index to main table
//	lua_pushlstring(L, "__nextIndex", 11);
//	lua_pushinteger(L, size + 1);
//	lua_rawset(L, -3);
//}

void LuaParserUtil::bsonDocumentToLuaTable(bson_iter_t* it, lua_State* L) {
    const int size = bson_iter_documentSize(it);
    lua_createtable(L, 0, size);
    while (bson_iter_next(it)) {
        lua_pushlstring(L, bson_iter_key(it), bson_iter_keyLength(it));
        bsonObjectToLua(it, L);
        lua_rawset(L, -3);
    }
}

void LuaParserUtil::bsonArrayToLua(bson_iter_t* it, lua_State* L) {
    const int size = bson_iter_documentSize(it);
    lua_createtable(L, size, 0);
    luaL_setmetatable(L, BSON_NAME_ARRAY);
    size_t index = 0;
    while (bson_iter_next(it)) {
        lua_pushinteger(L, ++index);
        bsonObjectToLua(it, L);
        lua_rawset(L, -3);
    }
}

void LuaParserUtil::bsonObjectToLua(bson_iter_t* it, lua_State* L) {
    switch (bson_iter_type(it)) {
    case BSON_TYPE_DOUBLE:
    {
        double tmp = bson_iter_double(it);
        int a = 2;
    }
    lua_pushnumber(L, bson_iter_double(it));
    break;
    case BSON_TYPE_UTF8:
        lua_pushlstring(L, bson_iter_utf8(it), bson_iter_utf8Length(it));
        break;
    case BSON_TYPE_DOCUMENT: {
        bson_iter_t childIt;
        if (bson_iter_recurse(it, &childIt)) {
            bsonDocumentToLua(&childIt, L);
        }
        break;
    }
    case BSON_TYPE_ARRAY: {
        bson_iter_t childIt;
        if (bson_iter_recurse(it, &childIt)) {
            bsonArrayToLua(&childIt, L);
        }
        break;
    }
    case BSON_TYPE_BINARY:
        LuaBsonTypes::lua_pushBinary(L, bson_iter_binary(it), bson_iter_binaryLength(it), bson_iter_binarySubType(it));
        break;
    case BSON_TYPE_UNDEFINED:
        lua_pushnil(L);
        break;
    case BSON_TYPE_OID:
        LuaBsonTypes::lua_pushObjectId(L, bson_iter_oid(it));
        break;
    case BSON_TYPE_BOOL:
        lua_pushboolean(L, bson_iter_bool(it));
        break;
    case BSON_TYPE_DATE_TIME:
        LuaBsonTypes::lua_pushIsoDate(L, bson_iter_timestamp(it));
        break;
    case BSON_TYPE_NULL:
        LuaBsonTypes::lua_pushNull(L);
        break;
    case BSON_TYPE_REGEX:
        LuaBsonTypes::lua_pushRegexp(L, bson_iter_regex(it), bson_iter_regexLength(it), bson_iter_regexOptions(it), bson_iter_regexOptionsLength(it));
        break;
    case BSON_TYPE_DBPOINTER:
        LuaBsonTypes::lua_pushDbPointer(L, bson_iter_dbpointer(it), bson_iter_dbpointerLength(it), bson_iter_dbpointerOID(it));
        break;
    case BSON_TYPE_CODE:
        LuaBsonTypes::lua_pushCode(L, bson_iter_code(it), bson_iter_codeLength(it));
        break;
    case BSON_TYPE_SYMBOL:
        LuaBsonTypes::lua_pushSymbol(L, bson_iter_symbol(it), bson_iter_symbolLength(it));
        break;
    case BSON_TYPE_CODEWSCOPE:
        LuaBsonTypes::lua_pushCodeWithScope(L, bson_iter_codewscope(it), bson_iter_codewscopeLength(it), bson_iter_codewscopeScope(it), bson_iter_codewscopeScopeLength(it));
        break;
    case BSON_TYPE_INT32:
        LuaBsonTypes::lua_pushNumberInt(L, bson_iter_int32(it));
        break;
    case BSON_TYPE_TIMESTAMP:
        LuaBsonTypes::lua_pushTimestamp(L, bson_iter_timestamp(it));
        break;
    case BSON_TYPE_INT64:
        LuaBsonTypes::lua_pushNumberLong(L, bson_iter_int64(it));
        break;
    case BSON_TYPE_DECIMAL128:
        LuaBsonTypes::lua_pushNumberDecimal(L, bson_iter_decimal128(it));
        break;
    case BSON_TYPE_MAXKEY:
        LuaBsonTypes::lua_pushMaxKey(L);
        break;
    case BSON_TYPE_MINKEY:
        LuaBsonTypes::lua_pushMinKey(L);
        break;
    default:
        luaL_error(L, "Unknown bson type '%d'", bson_iter_type(it));
    }
}

const int LuaParserUtil::bson_iter_documentSize(const bson_iter_t* it) {
    if (!it->raw) {
        return 0;
    }
    int size = 0;
    const uint8_t* data = it->raw;
    uint32_t o = it->next_off;
    uint32_t len = it->len;
    uint32_t type;
    while (true) {
        type = *(data + o);
        //Key length + type length
        o += (uint32_t)strlen((const char*)(data + o + 1)) + 2;
        if (o >= len) {
            return size;
        }
        switch (type) {
        case BSON_TYPE_DECIMAL128:
            o += 16;
            break;
        case BSON_TYPE_OID:
            o += 12;
            break;
        case BSON_TYPE_DATE_TIME:
        case BSON_TYPE_DOUBLE:
        case BSON_TYPE_INT64:
        case BSON_TYPE_TIMESTAMP:
            o += 8;
            break;
        case BSON_TYPE_INT32:
            o += 4;
            break;
        case BSON_TYPE_BOOL:
            o += 1;
            break;
        case BSON_TYPE_MAXKEY:
        case BSON_TYPE_MINKEY:
        case BSON_TYPE_NULL:
        case BSON_TYPE_UNDEFINED:
            //Offset 0
            break;
        case BSON_TYPE_CODE:
        case BSON_TYPE_SYMBOL:
        case BSON_TYPE_UTF8:
            //Length int + string length
            o += 4 + *(data + o);
            break;
        case BSON_TYPE_ARRAY:
        case BSON_TYPE_DOCUMENT:
            //Document size
            o += *(data + o);
            break;
        case BSON_TYPE_REGEX:
            //Regex length + options length
            o += (uint32_t)strlen((const char*)(data + o)) + 1;
            o += (uint32_t)strlen((const char*)(data + o)) + 1;
            break;
        case BSON_TYPE_BINARY:
            //Length int + subtype + bindata length
            o += 5 + *(data + o);
            break;
        case BSON_TYPE_DBPOINTER:
            //Length int + string length + oid
            o += 4 + *(data + o) + 12;
            break;
        case BSON_TYPE_CODEWSCOPE:
            //Code size
            o += *(data + o);
            break;
        default:
            return size;
        }
        if (o >= len) {
            return size;
        }
        ++size;
    }
    return size;
}

/*************************************
************** PRINT LUA *************
**************************************/

void LuaParserUtil::printLua(lua_State* L, const int i) {
    //If the object has a tostring meta method use it.
    if (luaL_callmeta(L, i, "__tostring")) {
        printf(lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }
    //Fall back to basic types.
    switch (lua_type(L, i)) {
    case LUA_TNONE:
    case LUA_TNIL:
        printf("undefined");
        break;
    case LUA_TBOOLEAN:
        printf(lua_toboolean(L, i) ? "true" : "false");
        break;
    case LUA_TNUMBER: {
        printf("%g", lua_tonumber(L, i));
        break;
    }
    case LUA_TSTRING:
        printf(lua_tostring(L, i));
        break;
    case LUA_TTABLE:
        if (luaL_testarray(L, i)) {
            printf("[object Array]");
        }
        else {
            printf("[object Object]");
        }
        break;
    case LUA_TFUNCTION:
        printf("function (...) { ... }");
        break;
    default:
        luaL_error(L, "Unknown LUA type '%s'", lua_typename(L, lua_type(L, -1)));
    }
}