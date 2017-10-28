#pragma once
#include "Lua.hpp"
#include "bson.h"

#define BSON_TYPE_WRITE_RESULT 0x20

#define BSON_NAME_NULL "null"
#define BSON_NAME_OID "ObjectId"
#define BSON_NAME_DECIMAL128 "NumberDecimal"
#define BSON_NAME_INT64 "NumberLong"
#define BSON_NAME_INT32 "NumberInt"
#define BSON_NAME_DATE_TIME "ISODate"
#define BSON_NAME_WRITE_RESULT "WriteResult"
#define BSON_NAME_BINARY "BinData"
#define BSON_NAME_REGEX "RegExp"
#define BSON_NAME_DOCUMENT "Document"
#define BSON_NAME_ARRAY "Array"
#define BSON_NAME_CODE "Code"
#define BSON_NAME_SYMBOL "Symbol"
#define BSON_NAME_TIMESTAMP "Timestamp"
#define BSON_NAME_MAXKEY "MaxKey"
#define BSON_NAME_MINKEY "MinKey"
#define BSON_NAME_DBPOINTER "DBPointer"
#define BSON_NAME_CODEWSCOPE "CodeWithScope"

class LuaBsonTypes {
public:
	static void lua_registerBsonTypes(lua_State *L);

	static void lua_pushObjectId(lua_State *L, const bson_oid_t *pOid);
	static const bson_oid_t* lua_readObjectId(lua_State *L, const int index);

	static void lua_pushNumberInt(lua_State *L, const int32_t value);
	static const int32_t lua_readNumberInt(lua_State *L, const int index);

	static void lua_pushNumberLong(lua_State *L, const int64_t value);
	static const int64_t lua_readNumberLong(lua_State *L, const int index);

	static void lua_pushNumberDecimal(lua_State *L, const bson_decimal128_t value);
	static const bson_decimal128_t* lua_readNumberDecimal(lua_State *L, const int index);

	static void lua_pushIsoDate(lua_State *L, const time_t value);
	static const time_t lua_readIsoDate(lua_State *L, const int index);

	static void lua_pushTimestamp(lua_State *L, const time_t value);
	static void lua_readTimestamp(lua_State *L, const int index, uint32_t *pTimestamp, uint32_t *pIncrement);

	static void lua_pushRegexp(lua_State *L, const char *pRegexp, const uint32_t lengthRegexp, const char *pOptions, const uint32_t lengthOptions);
	static void lua_readRegexp(lua_State *L, const int index, const char **pRegexp, const char **pOptions, uint32_t *pLengthRegexp = nullptr, uint32_t *pLengthOptions = nullptr);

	static void lua_pushBinary(lua_State *L, const uint8_t *pBinary, const uint32_t length, const bson_subtype_t subType = BSON_SUBTYPE_BINARY);
	static void lua_readBinary(lua_State *L, const int index, uint32_t *pLength, bson_subtype_t *pSubType, const uint8_t **pBinary);

	static void lua_pushSymbol(lua_State *L, const char *pSymbol, const uint32_t length);
	static void lua_readSymbol(lua_State *L, const int index, const char **pSymbol, uint32_t *pLength);
	
	static void lua_pushDbPointer(lua_State *L, const char *pCollection, const uint32_t length, const bson_oid_t *pOid);
	static void lua_readDbPointer(lua_State *L, const int index, const char **pCollection, const bson_oid_t **pOid);

	static void lua_pushCode(lua_State *L, const char *pCode, uint32_t length);
	static const char* lua_readCode(lua_State *L, const int index);

	static void lua_pushCodeWithScope(lua_State *L, const char *pCode, const uint32_t length, const uint8_t *pScope, const uint32_t scopeLength);
	static void lua_readCodeWithScope(lua_State *L, const int index, const char **pCode, const bson_t **pScope);

	static void lua_pushNull(lua_State *L);
	static void lua_pushMaxKey(lua_State *L);
	static void lua_pushMinKey(lua_State *L);
	static void lua_pushWriteResult(lua_State *L, const bson_t *pWR);
	static void lua_pushOkResult(lua_State *L);

	static int lua_documentNext(lua_State *L, int index);

private:
	static void lua_pushMetaTable(lua_State *L, const char *name, int bsonType, lua_CFunction constructor = nullptr, lua_CFunction toString = nullptr);
	static void lua_pushMetaTable(lua_State *L, const char *name, int bsonType, lua_CFunction constructor, const luaL_reg metaFuncs[]);

	static int lua_objectId(lua_State *L);
	static int lua_objectIdToString(lua_State *L);
	static int lua_objectIdValueOf(lua_State *L);
	static int lua_objectIdGetTimestamp(lua_State *L);

	static int lua_numberInt(lua_State *L);
	static int lua_numberIntToString(lua_State *L);
	static int lua_numberIntConcat(lua_State *L);
	static int lua_numberIntUnm(lua_State *L);
	static int lua_numberIntAdd(lua_State *L);
	static int lua_numberIntSub(lua_State *L);
	static int lua_numberIntMul(lua_State *L);
	static int lua_numberIntDiv(lua_State *L);
	static int lua_numberIntMod(lua_State *L);
	static int lua_numberIntPow(lua_State *L);
	static int lua_numberIntEq(lua_State *L);
	static int lua_numberIntLt(lua_State *L);
	static int lua_numberIntLe(lua_State *L);

	static int lua_numberLong(lua_State *L);
	static int lua_numberLongToString(lua_State *L);
	static int lua_numberLongConcat(lua_State *L);
	static int lua_numberLongUnm(lua_State *L);
	static int lua_numberLongAdd(lua_State *L);
	static int lua_numberLongSub(lua_State *L);
	static int lua_numberLongMul(lua_State *L);
	static int lua_numberLongDiv(lua_State *L);
	static int lua_numberLongMod(lua_State *L);
	static int lua_numberLongPow(lua_State *L);
	static int lua_numberLongEq(lua_State *L);
	static int lua_numberLongLt(lua_State *L);
	static int lua_numberLongLe(lua_State *L);

	static int lua_numberDecimalToString(lua_State *L);

	static int lua_isoDate(lua_State *L);
	static int lua_isoDateToString(lua_State *L);
	static int lua_isoDateValueOf(lua_State *L);

	static int lua_regExp(lua_State *L);
	static int lua_regExpToString(lua_State *L);

	static int lua_uuid(lua_State *L);
	static int lua_binaryToString(lua_State *L);

	static int lua_nullToString(lua_State *L);
	static int lua_symbolToString(lua_State *L);
	static int lua_timestampToString(lua_State *L);
	static int lua_dbPointerToString(lua_State *L);
	static int lua_maxKeyToString(lua_State *L);
	static int lua_minKeyToString(lua_State *L);
	static int lua_writeResultToString(lua_State *L);
	
};


