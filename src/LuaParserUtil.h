#pragma once
#include "Lua.hpp"
#include "mongoc.h"
#include "String.h"

class LuaParserUtil {
public:
	static void luaToBson(lua_State *L, bson_t *bsonOut, const int index);
	static void luaToJson(lua_State *L, String &jsonOut, int index, bool pretty = false, int depth = 0);
	static void bsonToLua(const bson_t *bsonIn, lua_State *L);
	static void bsonToLuaTable(const bson_t *bsonIn, lua_State *L);
	static void bsonSubToLua(const bson_t *bsonIn, lua_State *L, const char *field);

private:
	static void luaTableToBson(lua_State *L, bson_t *pBsonOut, int index);
	static void luaArrayToBson(lua_State *L, bson_t *pBsonOut, int index);
	static void luaDocumentToBson(lua_State *L, bson_t *pBsonOut, int index);
	static void luaObjectToBson(lua_State *L, bson_t *bsonOut, int index, const char*keyStr, int keyLength);

	static void luaTableToJson(lua_State *L, String &jsonOut, int index, bool pretty = false, int depth = 0);
	static void luaDocumentToJson(lua_State *L, String &jsonOut, int index, bool pretty = false, int depth = 0);
	static void luaArrayToJson(lua_State *L, String &jsonOut, int index, bool pretty = false, int depth = 0);
	static const String getIndent(int numIndents);

	static void bsonDocumentToLua(bson_iter_t *it, lua_State *L);
	static void bsonDocumentToLuaTable(bson_iter_t *it, lua_State *L);
	static void bsonArrayToLua(bson_iter_t *it, lua_State *L);
	static void bsonObjectToLua(bson_iter_t *it, lua_State *L);
	
	static const int bson_iter_documentSize(const bson_iter_t *it);

	static inline const bson_type_t bson_iter_type(const bson_iter_t *it) {
		return (bson_type_t)(it->raw + it->type)[0];
	}

	static inline const char* bson_iter_key(const bson_iter_t *it) {
		return (const char*)(it->raw + it->key);
	}

	static inline uint32_t bson_iter_keyLength(const bson_iter_t *it) {
		return (it->d1 != (uint32_t)-1 ? it->d1 - it->key : it->next_off - it->key) - 1;
	}

	static inline const char* bson_iter_regex(const bson_iter_t *it) {
		return (const char*)(it->raw + it->d1);
	}

	static inline const char* bson_iter_regexOptions(const bson_iter_t *it) {
		return (const char*)(it->raw + it->d2);
	}

	static inline const uint32_t bson_iter_regexLength(const bson_iter_t *it) {
		return it->d2 - it->d1 - 1;
	}

	static inline const uint32_t bson_iter_regexOptionsLength(const bson_iter_t *it) {
		return it->next_off - it->d2 - 1;
	}

	static inline const time_t bson_iter_timestamp(const bson_iter_t *it) {
		return *(time_t*)(it->raw + it->d1);
	}

	static inline const int32_t bson_iter_int32(const bson_iter_t *it) {
		return *(int32_t*)(it->raw + it->d1);
	}

	static inline const int64_t bson_iter_int64(const bson_iter_t *it) {
		return *(int64_t*)(it->raw + it->d1);
	}

	static inline const bson_decimal128_t bson_iter_decimal128(const bson_iter_t *it) {
		return *(bson_decimal128_t*)(it->raw + it->d1);
	}

	static inline const bool bson_iter_bool(const bson_iter_t *it) {
		return *(bool*)(it->raw + it->d1);
	}

	static inline const  bson_oid_t* bson_iter_oid(const bson_iter_t *it) {
		return(const bson_oid_t*)(it->raw + it->d1);
	}

	static inline const char* bson_iter_symbol(const bson_iter_t *it) {
		return (const char*)(it->raw + it->d2);
	}

	static inline const uint32_t bson_iter_symbolLength(const bson_iter_t *it) {
		return *(uint32_t*)(it->raw + it->d1) - 1;
	}
	
	static inline const char* bson_iter_code(const bson_iter_t *it) {
		return (const char*)(it->raw + it->d2);
	}

	static inline const uint32_t bson_iter_codeLength(const bson_iter_t *it) {
		return *(uint32_t*)(it->raw + it->d1) - 1;
	}

	static inline const char* bson_iter_codewscope(const bson_iter_t *it) {
		return (const char*)(it->raw + it->d3);
	}

	static inline const uint32_t bson_iter_codewscopeLength(const bson_iter_t *it) {
		return  *(uint32_t*)(it->raw + it->d2) - 1;
	}

	static inline const uint8_t* bson_iter_codewscopeScope(const bson_iter_t *it) {
		return (uint8_t*)(it->raw + it->d4);
	}

	static inline const uint32_t bson_iter_codewscopeScopeLength(const bson_iter_t *it) {
		return  *(uint32_t*)(it->raw + it->d4);
	}

	static inline const char* bson_iter_dbpointer(const bson_iter_t *it) {
		return (const char*)(it->raw + it->d2);
	}

	static inline const uint32_t bson_iter_dbpointerLength(const bson_iter_t *it) {
		return *(uint32_t*)(it->raw + it->d1) - 1;
	}

	static inline const bson_oid_t* bson_iter_dbpointerOID(const bson_iter_t *it) {
		return(const bson_oid_t*)(it->raw + it->d3);
	}

	static inline const char* bson_iter_utf8(const bson_iter_t *it) {
		return (const char*)(it->raw + it->d2);
	}

	static inline const uint32_t bson_iter_utf8Length(const bson_iter_t *it) {
		return *(uint32_t*)(it->raw + it->d1) - 1;
	}

	static inline const double bson_iter_double(const bson_iter_t *it) {
		return*(double*)(it->raw + it->d1);
	}

	static inline const uint8_t*  bson_iter_binary(const bson_iter_t *it) {
		return bson_iter_binarySubType(it) == BSON_SUBTYPE_BINARY_DEPRECATED ? it->raw + it->d3 + sizeof(int32_t) : it->raw + it->d3;
	}

	static inline const uint32_t bson_iter_binaryLength(const bson_iter_t *it) {
		return bson_iter_binarySubType(it) == BSON_SUBTYPE_BINARY_DEPRECATED ? *(uint32_t*)(it->raw + it->d1) - sizeof(int32_t) : *(uint32_t*)(it->raw + it->d1);
	}

	static inline const bson_subtype_t  bson_iter_binarySubType(const bson_iter_t *it) {
		return (bson_subtype_t) * (it->raw + it->d2);
	}

};

