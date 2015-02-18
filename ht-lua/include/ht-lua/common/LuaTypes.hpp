#ifndef __HTLUA_LUATYPES_HPP__
#define __HTLUA_LUATYPES_HPP__

namespace htlua {

struct LuaTypeEnd;

template <typename T>
struct LuaCallback;

template <typename T>
struct LuaType {
	enum { isValid = 0 };
	constexpr static const char *name = nullptr;

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst);
};

template <>
struct LuaType<void> {
	enum { isValid = 1 };
	constexpr static const char *name = "void";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return true;
	}
};

template <>
struct LuaType<bool> {
	enum { isValid = 1 };
	constexpr static const char *name = "bol";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return lua_isboolean(L, argIndex);;
	}

	static void pushValue(lua_State *L, bool value)
	{
		lua_pushboolean(L, value);
	}

	static void getValue(lua_State *L, int argIndex, bool &value)
	{
		value = lua_toboolean(L, argIndex);
	}
};

template <>
struct LuaType<char> {
	enum { isValid = 1 };
	constexpr static const char *name = "char";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return true;
	}
};

template <>
struct LuaType<int> {
	enum { isValid = 1 };
	constexpr static const char *name = "int";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
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
struct LuaType<uint8_t> {
	enum { isValid = 1 };
	constexpr static const char *name = "uint8_t";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return lua_isnumber(L, argIndex);
	}

	static void pushValue(lua_State *L, uint8_t value)
	{
		lua_pushnumber(L, value);
	}

	static void getValue(lua_State *L, int argIndex, uint8_t &value)
	{
		value = lua_tointeger(L, argIndex);
	}
};

template <>
struct LuaType<uint32_t> {
	enum { isValid = 1 };
	constexpr static const char *name = "uint32_t";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return lua_isnumber(L, argIndex);
	}

	static void pushValue(lua_State *L, uint32_t value)
	{
		lua_pushnumber(L, value);
	}

	static void getValue(lua_State *L, int argIndex, uint32_t &value)
	{
		value = lua_tointeger(L, argIndex);
	}
};

#ifdef __LP64__
template <>
struct LuaType<size_t> {
	enum { isValid = 1 };
	constexpr static const char *name = "size_t";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
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
#endif

template <>
struct LuaType<off64_t> {
	enum { isValid = 1 };
	constexpr static const char *name = "off64_t";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return lua_isnumber(L, argIndex);
	}

	static void pushValue(lua_State *L, off64_t value)
	{
		lua_pushnumber(L, value);
	}

	static void getValue(lua_State *L, int argIndex, off64_t &value)
	{
		value = lua_tointeger(L, argIndex);
	}
};

template <>
struct LuaType<std::string> {
	enum { isValid = 1 };
	constexpr static const char *name = "string";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return lua_isstring(L, argIndex);
	}

	static void pushValue(lua_State *L, const char *value)
	{
		lua_pushstring(L, value);
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
	enum { isValid = 1 };
	constexpr static const char *name = "u32string";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
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

template <typename R, typename... Args>
struct LuaType<LuaCallback<R(Args...)>> {
	enum { isValid = 1 };
	constexpr static const char *name = "LuaCallback";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return lua_isfunction(L, argIndex);
	}
};

} // namespace htlua

#endif // !__HTLUA_LUATYPES_HPP__
