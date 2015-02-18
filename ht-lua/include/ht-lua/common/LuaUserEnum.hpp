#ifndef __HTLUA_LUAUSERENUM_HPP__
#define __HTLUA_LUAUSERENUM_HPP__

namespace htlua {

template <typename T>
struct LuaUserEnum {
	enum { isValid = 1 };

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return lua_isnumber(L, argIndex);
	}

	static void pushValue(lua_State *L, T value)
	{
		lua_pushnumber(L, value);
	}

	static void getValue(lua_State *L, int argIndex, T &value)
	{
		value = (T) lua_tonumber(L, argIndex);
	}
};

} // namespace htlua

#endif // !__HTLUA_LUAUSERENUM_HPP__
