#include "LuaBsonTypes.h"
#include "mongoc.h"
#include "Date.h"
#include "LuaParserUtil.h"
#include "libbson_b64_pton.h"
#include <regex>
#include <cmath>

#define BUFFER_SIZE 128
#define OID_STR_SIZE 24

/*************************************
*************** PUBLIC ***************
**************************************/

void LuaBsonTypes::lua_registerBsonTypes(lua_State *L) {
	int count = lua_gettop(L);

	lua_pushMetaTable(L, BSON_NAME_REGEX, BSON_TYPE_REGEX, lua_regExp, lua_regExpToString);
	lua_pushMetaTable(L, BSON_NAME_DECIMAL128, BSON_TYPE_DECIMAL128, nullptr, lua_numberDecimalToString);	
	lua_pushMetaTable(L, BSON_NAME_WRITE_RESULT, BSON_TYPE_WRITE_RESULT, nullptr, lua_writeResultToString);
	lua_pushMetaTable(L, BSON_NAME_BINARY, BSON_TYPE_BINARY, nullptr, lua_binaryToString);
	lua_pushMetaTable(L, BSON_NAME_SYMBOL, BSON_TYPE_SYMBOL, nullptr, lua_symbolToString);
	lua_pushMetaTable(L, BSON_NAME_TIMESTAMP, BSON_TYPE_TIMESTAMP, nullptr, lua_timestampToString);
	lua_pushMetaTable(L, BSON_NAME_MAXKEY, BSON_TYPE_MAXKEY, nullptr, lua_maxKeyToString);
	lua_pushMetaTable(L, BSON_NAME_MINKEY, BSON_TYPE_MINKEY, nullptr, lua_minKeyToString);
	lua_pushMetaTable(L, BSON_NAME_DBPOINTER, BSON_TYPE_DBPOINTER, nullptr, lua_dbPointerToString);
	lua_pushMetaTable(L, BSON_NAME_CODE, BSON_TYPE_CODE);
	lua_pushMetaTable(L, BSON_NAME_CODEWSCOPE, BSON_TYPE_CODEWSCOPE);

	const luaL_reg numberIntMetaFuncs[] = {
		{ "__tostring", lua_numberIntToString },
		{ "__concat", lua_numberIntConcat },
		{ "__unm", lua_numberIntUnm },
		{ "__add", lua_numberIntAdd },
		{ "__sub", lua_numberIntSub },
		{ "__mul", lua_numberIntMul },
		{ "__div", lua_numberIntDiv },
		{ "__mod", lua_numberIntMod },
		{ "__pow", lua_numberIntPow },
		{ "__eq", lua_numberIntEq },
		{ "__lt", lua_numberIntLt },
		{ "__le", lua_numberIntLe },
		{ NULL, NULL }
	};
	lua_pushMetaTable(L, BSON_NAME_INT32, BSON_TYPE_INT32, lua_numberInt, numberIntMetaFuncs);

	const luaL_reg numberLongMetaFuncs[] = {
		{ "__tostring", lua_numberLongToString },
		{ "__concat", lua_numberLongConcat },
		{ "__unm", lua_numberLongUnm },
		{ "__add", lua_numberLongAdd },
		{ "__sub", lua_numberLongSub },
		{ "__mul", lua_numberLongMul },
		{ "__div", lua_numberLongDiv },
		{ "__mod", lua_numberLongMod },
		{ "__pow", lua_numberLongPow },
		{ "__eq", lua_numberLongEq },
		{ "__lt", lua_numberLongLt },
		{ "__le", lua_numberLongLe },
		{ NULL, NULL }
	};
	lua_pushMetaTable(L, BSON_NAME_INT64, BSON_TYPE_INT64, lua_numberLong, numberLongMetaFuncs);

	const luaL_reg isoDateMetaFuncs[] = {
		{ "__tostring", lua_isoDateToString },
		{ "toString", lua_isoDateToString },
		{ "valueOf", lua_isoDateValueOf },
		{ NULL, NULL }
	};
	lua_pushMetaTable(L, BSON_NAME_DATE_TIME, BSON_TYPE_DATE_TIME, lua_isoDate, isoDateMetaFuncs);

	const luaL_reg objectIdMetaFuncs[] = {
		{ "__tostring", lua_objectIdToString },
		{ "toString", lua_objectIdToString },
		{ "valueOf", lua_objectIdValueOf },
		{ "getTimestamp", lua_objectIdGetTimestamp },
		{ NULL, NULL }
	};
	lua_pushMetaTable(L, BSON_NAME_OID, BSON_TYPE_OID, lua_objectId, objectIdMetaFuncs);

	lua_pushMetaTable(L, BSON_NAME_NULL, BSON_TYPE_NULL, nullptr, lua_nullToString);
	lua_newuserdata(L, 0);
	luaL_setmetatable(L, BSON_NAME_NULL);
	lua_setglobal(L, BSON_NAME_NULL);

	//TODO
	//lua_register(L, "UUID", lua_uuid);

	lua_settop(L, count);
}

int LuaBsonTypes::lua_documentNext(lua_State *L, int index) {
	if (index < 0) {
		--index;
	}
	//Get order table.
	lua_pushlstring(L, "__order", 7);
	lua_rawget(L, index);
	//Create copy of next key
	lua_pushvalue(L, -2);
	//Remove old key;
	lua_remove(L, -3);
	//Get next key
	if (lua_next(L, -2)) {
		//Remove order table.
		lua_remove(L, -3);
		//Get next value.
		lua_pushvalue(L, -1);
		if (index < 0) {
			--index;
		}
		lua_rawget(L, index);
		return 3;
	}
	//Remove order table.
	lua_pop(L, 1);
	return 0;
}

/*************************************
*************** PRIVATE **************
**************************************/

void LuaBsonTypes::lua_pushMetaTable(lua_State *L, const char *name, const int bsonType, lua_CFunction constructor, lua_CFunction toString) {
	if (luaL_newmetatable(L, name)) {
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_setStr(L, "[Protected metatable]", "__metatable");
		lua_setInt(L, bsonType, "__bson");
		if (toString) {
			lua_setFunc(L, toString, "__tostring");
		}
	}
	if (constructor) {
		lua_register(L, name, constructor);
	}
}

void LuaBsonTypes::lua_pushMetaTable(lua_State *L, const char *name, int bsonType, lua_CFunction constructor, const luaL_reg metaFuncs[]) {
	if (luaL_newmetatable(L, name)) {
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_setStr(L, "Protected", "__metatable");
		lua_setInt(L, bsonType, "__bson");
		lua_setFuncs(L, metaFuncs);
	}
	if (constructor) {
		lua_register(L, name, constructor);
	}
}

/*************************************
************** LUA BIND **************
**************************************/

/*************** OBJECT ID ***************/

int LuaBsonTypes::lua_objectId(lua_State *L) {
	bson_oid_t oid;
	//Generate OID string.
	if (lua_gettop(L) == 0) {
		bson_oid_init(&oid, NULL);
	}
	else {
		bson_oid_init_from_string(&oid, lua_tostring(L, 1));
	}
	lua_pushObjectId(L, &oid);
	return 1;
}

void LuaBsonTypes::lua_pushObjectId(lua_State *L, const bson_oid_t *pOid) {
	memcpy(lua_newuserdata(L, sizeof(bson_oid_t)), pOid, sizeof(bson_oid_t));
	luaL_setmetatable(L, BSON_NAME_OID);
}

const bson_oid_t* LuaBsonTypes::lua_readObjectId(lua_State *L, const int index) {
	return (bson_oid_t*)lua_touserdata(L, index);
}

int LuaBsonTypes::lua_objectIdToString(lua_State *L) {
	const bson_oid_t *pOid = lua_readObjectId(L, 1);
	char oidStr[OID_STR_SIZE + 1];
	bson_oid_to_string(pOid, oidStr);
	char buf[37];
	snprintf(buf, 37, "%s(\"%s\")", BSON_NAME_OID, oidStr);
	lua_pushlstring(L, buf, 36);
	return 1;
}

int LuaBsonTypes::lua_objectIdValueOf(lua_State *L) {
	const bson_oid_t *pOid = lua_readObjectId(L, 1);
	char oidStr[OID_STR_SIZE + 1];
	bson_oid_to_string(pOid, oidStr);
	lua_pushlstring(L, oidStr, OID_STR_SIZE);
	return 1;
}

int LuaBsonTypes::lua_objectIdGetTimestamp(lua_State *L) {
	const bson_oid_t *pOid = lua_readObjectId(L, 1);
	time_t time = bson_oid_get_time_t(pOid);
	lua_pushIsoDate(L, time * 1000);
	return 1;
}


/*************** NUMBER INT ***************/

int LuaBsonTypes::lua_numberInt(lua_State *L) {
	if (lua_gettop(L) > 0) {
		lua_pushNumberInt(L, (int)lua_tointeger(L, 1));
	}
	else {
		lua_pushNumberInt(L, 0);
	}
	return 1;
}

void LuaBsonTypes::lua_pushNumberInt(lua_State *L, const int32_t value) {
	*(int32_t*)lua_newuserdata(L, sizeof(int32_t)) = value;
	luaL_setmetatable(L, BSON_NAME_INT32);
}

const int32_t LuaBsonTypes::lua_readNumberInt(lua_State *L, const int index) {
	return *(int32_t*)lua_touserdata(L, index);
}

int LuaBsonTypes::lua_numberIntToString(lua_State *L) {
	char buf[BUFFER_SIZE];
	int length = snprintf(buf, BUFFER_SIZE, "%s(%d)", BSON_NAME_INT32, lua_readNumberInt(L, 1));
	lua_pushlstring(L, buf, length);
	return 1;
}

int LuaBsonTypes::lua_numberIntConcat(lua_State *L) {
	char buf[BUFFER_SIZE];
	int length = snprintf(buf, BUFFER_SIZE, "%s%d", lua_tostring(L, 1), lua_readNumberInt(L, 2));
	lua_pushlstring(L, buf, length);
	return 1;
}

int LuaBsonTypes::lua_numberIntUnm(lua_State *L) {
	lua_pushnumber(L, -lua_readNumberInt(L, 1));
	return 1;
}

int LuaBsonTypes::lua_numberIntAdd(lua_State *L) {
	lua_pushnumber(L, lua_readNumberInt(L, 1) + lua_tonumber(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberIntSub(lua_State *L) {
	lua_pushnumber(L, lua_readNumberInt(L, 1) - lua_tonumber(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberIntMul(lua_State *L) {
	lua_pushnumber(L, lua_readNumberInt(L, 1) * lua_tonumber(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberIntDiv(lua_State *L) {
	lua_pushnumber(L, lua_readNumberInt(L, 1) / lua_tonumber(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberIntMod(lua_State *L) {
	lua_pushnumber(L, std::fmod(lua_readNumberInt(L, 1), lua_tonumber(L, 2)));
	return 1;
}

int LuaBsonTypes::lua_numberIntPow(lua_State *L) {
	lua_pushnumber(L, std::pow(lua_readNumberInt(L, 1), lua_tonumber(L, 2)));
	return 1;
}

int LuaBsonTypes::lua_numberIntEq(lua_State *L) {
	lua_pushboolean(L, lua_readNumberInt(L, 1) == lua_readNumberInt(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberIntLt(lua_State *L) {
	lua_pushboolean(L, lua_readNumberInt(L, 1) < lua_readNumberInt(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberIntLe(lua_State *L) {
	lua_pushboolean(L, lua_readNumberInt(L, 1) <= lua_readNumberInt(L, 2));
	return 1;
}


/*************** NUMBER LONG ***************/

int LuaBsonTypes::lua_numberLong(lua_State *L) {
	if (lua_gettop(L) > 0) {
		switch (lua_type(L, 1)) {
		case LUA_TSTRING: {
			size_t strLength;
			const char *str = lua_tolstring(L, 1, &strLength);
			char *end;
			const int64_t value = strtoll(str, &end, 10);
			if (strLength == 0 || *end != '\0') {
				luaL_error(L, "NumberLong() error: Could not convert string '%s' to long long", str);
			}
			lua_pushNumberLong(L, value);
			return 1;
		}
		case LUA_TNUMBER:
			lua_pushNumberLong(L, lua_tointeger(L, 1));
			return 1;
		default:
			luaL_typerror(L, 1, "string|number");
		}
	}
	lua_pushNumberLong(L, 0LL);
	return 1;
}

void LuaBsonTypes::lua_pushNumberLong(lua_State *L, const int64_t value) {
	*(int64_t*)lua_newuserdata(L, sizeof(int64_t)) = value;
	luaL_setmetatable(L, BSON_NAME_INT64);
}

const int64_t LuaBsonTypes::lua_readNumberLong(lua_State *L, const int index) {
	return *(int64_t*)lua_touserdata(L, index);
}

int LuaBsonTypes::lua_numberLongToString(lua_State *L) {
	int64_t value = lua_readNumberLong(L, 1);
	char buf[BUFFER_SIZE];
	int length;
	if (abs(value) > INT_MAX) {
		length = snprintf(buf, BUFFER_SIZE, "%s(\"%ld\")", BSON_NAME_INT64, value);
	}
	else {
		length = snprintf(buf, BUFFER_SIZE, "%s(%ld)", BSON_NAME_INT64, value);
	}
	lua_pushlstring(L, buf, length);
	return 1;
}

int LuaBsonTypes::lua_numberLongConcat(lua_State *L) {
	char buf[BUFFER_SIZE];
	int length = snprintf(buf, BUFFER_SIZE, "%s%ld", lua_tostring(L, 1), lua_readNumberLong(L, 2));
	lua_pushlstring(L, buf, length);
	return 1;
}

int LuaBsonTypes::lua_numberLongUnm(lua_State *L) {
	lua_pushnumber(L, -(lua_Number)lua_readNumberLong(L, 1));
	return 1;
}

int LuaBsonTypes::lua_numberLongAdd(lua_State *L) {
	lua_pushnumber(L, lua_readNumberLong(L, 1) + lua_tonumber(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberLongSub(lua_State *L) {
	lua_pushnumber(L, lua_readNumberLong(L, 1) - lua_tonumber(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberLongMul(lua_State *L) {
	lua_pushnumber(L, lua_readNumberLong(L, 1) * lua_tonumber(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberLongDiv(lua_State *L) {
	lua_pushnumber(L, lua_readNumberLong(L, 1) / lua_tonumber(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberLongMod(lua_State *L) {
	lua_pushnumber(L, fmod(lua_readNumberLong(L, 1), lua_tonumber(L, 2)));
	return 1;
}

int LuaBsonTypes::lua_numberLongPow(lua_State *L) {
	lua_pushnumber(L, pow(lua_readNumberLong(L, 1), lua_tonumber(L, 2)));
	return 1;
}

int LuaBsonTypes::lua_numberLongEq(lua_State *L) {
	lua_pushboolean(L, lua_readNumberLong(L, 1) == lua_readNumberLong(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberLongLt(lua_State *L) {
	lua_pushboolean(L, lua_readNumberLong(L, 1) < lua_readNumberLong(L, 2));
	return 1;
}

int LuaBsonTypes::lua_numberLongLe(lua_State *L) {
	lua_pushboolean(L, lua_readNumberLong(L, 1) <= lua_readNumberLong(L, 2));
	return 1;
}


/*************** NUMBER DECIMAL ***************/

void LuaBsonTypes::lua_pushNumberDecimal(lua_State *L, const bson_decimal128_t value) {
	*(bson_decimal128_t*)lua_newuserdata(L, sizeof(bson_decimal128_t)) = value;
	luaL_setmetatable(L, BSON_NAME_DECIMAL128);
}

const bson_decimal128_t* LuaBsonTypes::lua_readNumberDecimal(lua_State *L, const int index) {
	return (bson_decimal128_t*)lua_touserdata(L, index);
}

int LuaBsonTypes::lua_numberDecimalToString(lua_State *L) {
	const bson_decimal128_t *pValue = lua_readNumberDecimal(L, 1);
	char decStr[BSON_DECIMAL128_STRING];
	bson_decimal128_to_string(pValue, decStr);
	char buf[BUFFER_SIZE];
	int length = snprintf(buf, BUFFER_SIZE, "%s(\"%s\")", BSON_NAME_DECIMAL128, decStr);
	lua_pushlstring(L, buf, length);
	return 1;
}


/*************** ISO DATE ***************/

int LuaBsonTypes::lua_isoDate(lua_State *L) {
	//No arg. Generate timestamp.
	if (lua_gettop(L) == 0) {
		lua_pushIsoDate(L, Date::getCurrentTimeMillis());
		return 1;
	}
	//String arg. Convert to timestamp.
	else{
		switch (lua_type(L, 1)) {
		case LUA_TSTRING: {
			time_t value;
			if (!Date::fromIsoString(lua_tostring(L, 1), &value)) {
				luaL_error(L, "Invalid ISO date string '%s'", lua_tostring(L, 1));
			}
			lua_pushIsoDate(L, value);
			return 1;
		}
		case LUA_TNUMBER:
			lua_pushIsoDate(L, lua_tointeger(L, 1));
			return 1;
		default:
			luaL_typerror(L, 1, "string|number");
		}
	}
	lua_pushIsoDate(L, 0);
	return 1;
}

void LuaBsonTypes::lua_pushIsoDate(lua_State *L, const time_t value) {
	*(time_t*)lua_newuserdata(L, sizeof(time_t)) = value;
	luaL_setmetatable(L, BSON_NAME_DATE_TIME);
}

const time_t LuaBsonTypes::lua_readIsoDate(lua_State *L, const int index) {
	return *(time_t*)lua_touserdata(L, index);
}

int LuaBsonTypes::lua_isoDateToString(lua_State *L) {
	time_t value = lua_readIsoDate(L, 1);
	String isoString = Date::toIsoString(value);
	char buf[BUFFER_SIZE];
	size_t length = snprintf(buf, BUFFER_SIZE, "%s(\"%s\")", BSON_NAME_DATE_TIME, isoString.c_str());
	lua_pushlstring(L, buf, length);
	return 1;
}

int LuaBsonTypes::lua_isoDateValueOf(lua_State *L) {
	lua_pushinteger(L, lua_readIsoDate(L, 1));
	return 1;
}


/*************** TIMESTAMP ***************/

void LuaBsonTypes::lua_pushTimestamp(lua_State *L, const time_t value) {
	*(time_t*)lua_newuserdata(L, sizeof(time_t)) = value;
	luaL_setmetatable(L, BSON_NAME_TIMESTAMP);
}

void LuaBsonTypes::lua_readTimestamp(lua_State *L, const int index, uint32_t *pTimestamp, uint32_t *pIncrement) {
	time_t value = *(time_t*)lua_touserdata(L, index);
	*pTimestamp = (value >> 32) & 0xFFFFFFFF;
	*pIncrement = value & 0xFFFFFFFF;
}

int LuaBsonTypes::lua_timestampToString(lua_State *L) {
	uint32_t timestamp, increment;
	lua_readTimestamp(L, 1, &timestamp, &increment);
	char buf[BUFFER_SIZE];
	size_t length = snprintf(buf, BUFFER_SIZE, "%s(%d, %d)", BSON_NAME_TIMESTAMP, timestamp, increment);
	lua_pushlstring(L, buf, length);
	return 1;
}


/*************** REG EXP ***************/

int LuaBsonTypes::lua_regExp(lua_State *L) {
	const int count = lua_gettop(L);
	const char *pRegExp;
	String options;
	size_t lengthRegexp, lengthOptions;
	if (count > 0) {
		pRegExp = luaL_validatelstring(L, 1, &lengthRegexp);
		if (!lengthRegexp) {
			pRegExp = "(?:)";
			lengthRegexp = 4;
		}
	}
	else {
		pRegExp = "(?:)";
		lengthRegexp = 4;
	}
	if (count > 1) {
		options = luaL_validatelstring(L, 2, &lengthOptions);
		for (int i = 0; i < lengthOptions; ++i) {
			switch (options[i]) {
			case 'i': case 'm': break;
			default:
				luaL_error(L, "SyntaxError: invalid regular expression flag '%c'", options[i]);
			}
		}
		if (lengthOptions > 1) {
			std::sort(options.begin(), options.end());
		}
	}
	else {
		options = "";
		lengthOptions = 0;
	}
	lua_pushRegexp(L, pRegExp, (uint32_t)lengthRegexp, options.c_str(), (uint32_t)lengthOptions);
	return 1;
}

void LuaBsonTypes::lua_pushRegexp(lua_State *L, const char *pRegexp, const uint32_t lengthRegexp, const char *pOptions, const uint32_t lengthOptions) {
	//uint32_t + char* + uint32_t + char*
	uint8_t *p = (uint8_t*)lua_newuserdata(L, sizeof(uint32_t) * 2 + lengthRegexp + lengthOptions + 2);
	memcpy(p, &lengthRegexp, sizeof(uint32_t));
	memcpy(p + sizeof(uint32_t), pRegexp, lengthRegexp + 1);
	memcpy(p + sizeof(uint32_t) + lengthRegexp + 1, &lengthOptions, sizeof(uint32_t));
	memcpy(p + sizeof(uint32_t) * 2 + lengthRegexp + 1, pOptions, lengthOptions + 1);
	luaL_setmetatable(L, BSON_NAME_REGEX);
}

void LuaBsonTypes::lua_readRegexp(lua_State *L, const int index, const char **pRegexp, const char **pOptions, uint32_t *pLengthRegexp, uint32_t *pLengthOptions) {
	uint32_t lengthRegexp;
	const char *p = (char*)lua_touserdata(L, index);
	lengthRegexp = *(uint32_t*)p;
	*pRegexp = p + sizeof(uint32_t);
	*pOptions = p + 2 * sizeof(uint32_t) + lengthRegexp + 1;
	if (pLengthRegexp) {
		memcpy(pLengthRegexp, &lengthRegexp, sizeof(uint32_t));
	}
	if (pLengthOptions) {
		memcpy(pLengthOptions, p + sizeof(uint32_t) + *pLengthRegexp + 1, sizeof(uint32_t));
	}
}

int LuaBsonTypes::lua_regExpToString(lua_State *L) {
	const char *pRegexp, *pOptions;
	lua_readRegexp(L, 1, &pRegexp, &pOptions);
	char buf[BUFFER_SIZE];
	size_t length = snprintf(buf, BUFFER_SIZE, "/%s/%s", pRegexp, pOptions);
	lua_pushlstring(L, buf, length);
	return 1;
}


/*************** BINARY ***************/


//static String HexToBase64(String hex) {
//	const char *base64Digits = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//	String base64 = "";
//	int group;
//	for (int i = 0; i < 30; i += 6) {
//		group = stoi(hex.substr(i, 6), nullptr, 16);
//		base64 += base64Digits[(group >> 18) & 0x3f];
//		base64 += base64Digits[(group >> 12) & 0x3f];
//		base64 += base64Digits[(group >> 6) & 0x3f];
//		base64 += base64Digits[group & 0x3f];
//	}
//	group = stoi(hex.substr(30, 2), nullptr, 16);
//	base64 += base64Digits[(group >> 2) & 0x3f];
//	base64 += base64Digits[(group << 4) & 0x3f];
//	base64 += "==";
//	return base64;
//}
//
//static String Base64ToHex(String base64) {
//	String base64Digits = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
//	const char *hexDigits = "0123456789abcdef";
//	String hex = "";
//	for (int i = 0; i < 24; ) {
//		int e1 = base64Digits.find(base64[i++]);
//		int e2 = base64Digits.find(base64[i++]);
//		int e3 = base64Digits.find(base64[i++]);
//		int e4 = base64Digits.find(base64[i++]);
//		int c1 = (e1 << 2) | (e2 >> 4);
//		int c2 = ((e2 & 15) << 4) | (e3 >> 2);
//		int c3 = ((e3 & 3) << 6) | e4;
//		hex += hexDigits[c1 >> 4];
//		hex += hexDigits[c1 & 15];
//		if (e3 != 64) {
//			hex += hexDigits[c2 >> 4];
//			hex += hexDigits[c2 & 15];
//		}
//		if (e4 != 64) {
//			hex += hexDigits[c3 >> 4];
//			hex += hexDigits[c3 & 15];
//		}
//	}
//	return hex;
//}

//int LuaBsonTypes::lua_uuid(lua_State *L) {
//	size_t length;
//	const char *pUuid = luaL_validatelstring(L, 1, &length);
//	if (length != 32) {
//		luaL_error(L, "Error: UUID string must have 32 characters");
//	}
//	String binary = HexToBase64(pUuid);
//	//lua_pushBinary(L, (const uint8_t*)binary.c_str(), (uint32_t)binary.size(), BSON_SUBTYPE_UUID);
//	size_t binaryLength = b64_pton(binary.c_str(), nullptr, 0);
//	uint8_t *pBinary = (uint8_t *)bson_malloc0(binaryLength);
//	b64_pton(pUuid, pBinary, binaryLength);
//	//int a = strlen((const char*)pBinary);
//	lua_pushBinary(L, pBinary, (uint32_t)binaryLength, BSON_SUBTYPE_UUID);
//	bson_free(pBinary);
//	return 1;
//}

void LuaBsonTypes::lua_pushBinary(lua_State *L, const uint8_t *pBinary, const uint32_t length, const bson_subtype_t subType) {
	//uint32_t + bson_subtype_t + uint8_t*
	const char *tmp = (const char *)pBinary;
	uint8_t *p = (uint8_t*)lua_newuserdata(L, sizeof(uint32_t) + sizeof(bson_subtype_t) + length);
	memcpy(p, &length, sizeof(uint32_t));
	memcpy(p + sizeof(uint32_t), &subType, sizeof(bson_subtype_t));
	memcpy(p + sizeof(uint32_t) + sizeof(bson_subtype_t), pBinary, length);
	luaL_setmetatable(L, BSON_NAME_BINARY);
}

void LuaBsonTypes::lua_readBinary(lua_State *L, const int index, uint32_t *pLength, bson_subtype_t *pSubType, const uint8_t **pBinary) {
	const uint8_t *p = (const uint8_t*)lua_touserdata(L, index);
	memcpy(pLength, p, sizeof(uint32_t));
	memcpy(pSubType, p + sizeof(uint32_t), sizeof(bson_subtype_t));
	*pBinary = p + sizeof(uint32_t) + sizeof(bson_subtype_t);
}

int LuaBsonTypes::lua_binaryToString(lua_State *L) {
	uint32_t binaryLength;
	bson_subtype_t subType;
	const uint8_t *pBinary;
	lua_readBinary(L, 1, &binaryLength, &subType, &pBinary);
	char buf[BUFFER_SIZE];
	size_t b64Length = (binaryLength / 3 + 1) * 4 + 1;
	char *b64 = (char *)bson_malloc0(b64Length);
	b64_ntop(pBinary, binaryLength, b64, b64Length);
	size_t stringLength;
	stringLength = snprintf(buf, BUFFER_SIZE, "%s(%d, \"%s\")", BSON_NAME_BINARY, subType, b64);
	bson_free(b64);
	lua_pushlstring(L, buf, stringLength);
	return 1;
	//switch (subType) {
	////case BSON_SUBTYPE_UUID_DEPRECATED:
	////	stringLength = snprintf(buf, BUFFER_SIZE, "LUUID(\"%s\")", b64);
	////	break;
	////case BSON_SUBTYPE_UUID:
	////	stringLength = snprintf(buf, BUFFER_SIZE, "UUID(\"%s\")", b64);
	////	break;
	//default:
	//	stringLength = snprintf(buf, BUFFER_SIZE, "%s(%d, \"%s\")", BSON_NAME_BINARY, subType, b64);
	//}
}


/*************** SYMBOL ***************/

void LuaBsonTypes::lua_pushSymbol(lua_State *L, const char *pSymbol, const uint32_t length) {
	uint8_t *p = (uint8_t*)lua_newuserdata(L, sizeof(uint32_t) + length);
	memcpy(p, &length, sizeof(length));
	memcpy(p + sizeof(length), pSymbol, length);
	luaL_setmetatable(L, BSON_NAME_SYMBOL);
}

void LuaBsonTypes::lua_readSymbol(lua_State *L, const int index, const char **pSymbol, uint32_t *pLength) {
	const char *p = (const char*)lua_touserdata(L, index);
	memcpy(pLength, p, sizeof(uint32_t));
	*pSymbol = p + sizeof(uint32_t);
}

int LuaBsonTypes::lua_symbolToString(lua_State *L) {
	const char *pSymbol;
	uint32_t length;
	lua_readSymbol(L, 1, &pSymbol, &length);
	lua_pushlstring(L, pSymbol, length);
	return 1;
}


/*************** DB POINTER ***************/

void LuaBsonTypes::lua_pushDbPointer(lua_State *L, const char *pCollection, const uint32_t length, const bson_oid_t *pOid) {
	uint8_t *p = (uint8_t*)lua_newuserdata(L, sizeof(uint32_t) + length + 1 + sizeof(bson_oid_t));
	memcpy(p, &length, sizeof(uint32_t));
	memcpy(p + sizeof(uint32_t), pCollection, length + 1);
	memcpy(p + sizeof(uint32_t) + length + 1, pOid, sizeof(bson_oid_t));
	luaL_setmetatable(L, BSON_NAME_DBPOINTER);
}

void LuaBsonTypes::lua_readDbPointer(lua_State *L, const int index, const char **pCollection, const bson_oid_t **pOid) {
	const char *p = (const char*)lua_touserdata(L, index);
	uint32_t length = *(uint32_t*)p;
	*pCollection = p + sizeof(uint32_t);
	*pOid = (bson_oid_t*)(p + sizeof(uint32_t) + length + 1);
}

int LuaBsonTypes::lua_dbPointerToString(lua_State *L) {
	const char *pCollection;
	const bson_oid_t *pOid;
	lua_readDbPointer(L, 1, &pCollection, &pOid);
	char oidStr[OID_STR_SIZE + 1];
	bson_oid_to_string(pOid, oidStr);
	char buf[BUFFER_SIZE];
	size_t length = snprintf(buf, BUFFER_SIZE, "%s(\"%s\", %s(\"%s\"))", BSON_NAME_DBPOINTER, pCollection, BSON_NAME_OID, oidStr);
	lua_pushlstring(L, buf, length);
	return 1;
}


/*************** CODE ***************/

void LuaBsonTypes::lua_pushCode(lua_State *L, const char *pCode, const uint32_t length) {
	lua_createtable(L, 0, 1);
	luaL_setmetatable(L, BSON_NAME_CODE);
	lua_pushlstring(L, "code", 4);
	lua_pushlstring(L, pCode, length);
	lua_rawset(L, -3);
}

const char* LuaBsonTypes::lua_readCode(lua_State *L, const int index) {
	lua_pushlstring(L, "code", 4);
	lua_rawget(L, index < 0 ? index - 1 : index);
	const char *pCode = lua_tostring(L, -1);
	lua_pop(L, 1);
	return pCode;
}


/*************** CODE WITH SCOPE ***************/

void LuaBsonTypes::lua_pushCodeWithScope(lua_State *L, const char *pCode, uint32_t length, const uint8_t *pScope, uint32_t scopeLength) {
	bson_t scopeBson;
	bson_init_static(&scopeBson, pScope, scopeLength);
	lua_createtable(L, 0, 2);
	luaL_setmetatable(L, BSON_NAME_CODEWSCOPE);
	lua_pushlstring(L, "code", 4);
	lua_pushlstring(L, pCode, length);
	lua_rawset(L, -3);
	lua_pushlstring(L, "scope", 5);
	LuaParserUtil::bsonToLua(&scopeBson, L);
	lua_rawset(L, -3);
}

void LuaBsonTypes::lua_readCodeWithScope(lua_State *L, const int index, const char **pCode, const bson_t **pScope) {
	
	
	//LuaParserUtil::luaToBson

	//bson_t scopeBson;
	//bson_init_static(&scopeBson, pScope, scopeLength);
}


/*************** NULL ***************/

void LuaBsonTypes::lua_pushNull(lua_State *L) {
	lua_getglobal(L, BSON_NAME_NULL);
}

int LuaBsonTypes::lua_nullToString(lua_State *L) {
	lua_pushlstring(L, BSON_NAME_NULL, 4);
	return 1;
}


/*************** MIN / MAX KEY ***************/

void LuaBsonTypes::lua_pushMaxKey(lua_State *L) {
	lua_newuserdata(L, 0);
	luaL_setmetatable(L, BSON_NAME_MAXKEY);
}

void LuaBsonTypes::lua_pushMinKey(lua_State *L) {
	lua_newuserdata(L, 0);
	luaL_setmetatable(L, BSON_NAME_MINKEY);
}

int LuaBsonTypes::lua_maxKeyToString(lua_State *L) {
	lua_pushlstring(L, "[BSON MaxKey]", 13);
	return 1;
}

int LuaBsonTypes::lua_minKeyToString(lua_State *L) {
	lua_pushlstring(L, "[BSON MinKey]", 13);
	return 1;
}


/*************** WRITE RESULT ***************/

void LuaBsonTypes::lua_pushWriteResult(lua_State *L, const bson_t *pWR) {
	LuaParserUtil::bsonToLuaTable(pWR, L);
	luaL_setmetatable(L, BSON_NAME_WRITE_RESULT);
}

int LuaBsonTypes::lua_writeResultToString(lua_State *L) {
	String json;
	LuaParserUtil::luaToJson(L, json, 1);
	char buf[BUFFER_SIZE];
	size_t length = snprintf(buf, BUFFER_SIZE, "%s(%s)", BSON_NAME_WRITE_RESULT, json.c_str());
	lua_pushlstring(L, buf, length);
	return 1;
}

void LuaBsonTypes::lua_pushOkResult(lua_State *L) {
	lua_createtable(L, 0, 1);
	lua_pushlstring(L, "ok", 2);
	lua_pushinteger(L, 1);
	lua_rawset(L, -3);
}
