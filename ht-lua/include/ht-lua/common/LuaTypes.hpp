#ifndef __HTLUA_LUATYPES_HPP__
#define __HTLUA_LUATYPES_HPP__

#include <string>
#include <tuple>
#include <ht/Utils.hpp>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/Sequence.hpp>

namespace htlua {

struct LuaTypeEnd;

template <typename T>
struct LuaType {
	static const bool known = false;
	constexpr static const char *name = nullptr;
};

template <>
struct LuaType<void> {
	static const bool known = true;
	constexpr static const char *name = "void";

	static bool isParamValid(lua_State *L, int argIndex)
	{
		return true;
	}
};

template <>
struct LuaType<char> {
	static const bool known = true;
	constexpr static const char *name = "char";

	static bool isParamValid(lua_State *L, int argIndex)
	{
		return true;
	}
};

template <>
struct LuaType<int> {
	static const bool known = true;
	constexpr static const char *name = "int";

	static bool isParamValid(lua_State *L, int argIndex)
	{
		return lua_isnumber(L, argIndex);
	}

	static void pushValue(lua_State *L, int value)
	{
		lua_pushnumber(L, value);
	}

	static void getValue(lua_State *L, int argIndex, int &value)
	{
		value = lua_tointeger(L, argIndex);
	}
};

template <>
struct LuaType<size_t> {
	static const bool known = true;
	constexpr static const char *name = "size_t";

	static bool isParamValid(lua_State *L, int argIndex)
	{
		return lua_isnumber(L, argIndex);
	}

	static void pushValue(lua_State *L, size_t value)
	{
		lua_pushnumber(L, value);
	}

	static void getValue(lua_State *L, int argIndex, size_t &value)
	{
		value = lua_tointeger(L, argIndex);
	}
};

template <>
struct LuaType<std::string> {
	static const bool known = true;
	constexpr static const char *name = "string";

	static bool isParamValid(lua_State *L, int argIndex)
	{
		return lua_isstring(L, argIndex);
	}

	static void pushValue(lua_State *L, std::string &value)
	{
		lua_pushstring(L, value.c_str());
	}

	static void getValue(lua_State *L, int argIndex, std::string &value)
	{
		value = lua_tostring(L, argIndex);
	}
};

template <>
struct LuaType<std::u32string> {
	static const bool known = true;
	constexpr static const char *name = "u32string";

	static bool isParamValid(lua_State *L, int argIndex)
	{
		return lua_isstring(L, argIndex);
	}

	static void pushValue(lua_State *L, std::u32string &u32value)
	{
		std::string value;

		value = ht::convertToEnvStr(u32value);
		lua_pushstring(L, value.c_str());
	}

	static void getValue(lua_State *L, int argIndex, std::u32string &u32value)
	{
		std::string value = lua_tostring(L, argIndex);

		u32value = ht::convertFromEnvStr(value);
	}
};

} // namespace htlua

#endif // !__HTLUA_LUATYPES_HPP__