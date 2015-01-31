#ifndef __HTLUA_LUAUSERTYPES_HPP__
#define __HTLUA_LUAUSERTYPES_HPP__

#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>

namespace htlua {

template <typename T>
struct LuaUserType {
	enum { isValid = 1 };

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<T>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, T *t)
	{
		LuaClass<T>::forwardReference(L, t);
	}

	static void pushValue(lua_State *L, const T *t)
	{
		LuaClass<T>::forwardReference(L, t);
	}

	static void getValue(lua_State *L, int argIndex, T *&t, bool takeOwnership)
	{
		LuaObjectParam<T>::getValue(L, argIndex, t, takeOwnership);
	}

	static void getValue(lua_State *L, int argIndex, const T *&t)
	{
		LuaObjectParam<T>::getValue(L, argIndex, t);
	}
};

} // namespace htlua

#endif // !__HTLUA_LUAUSERTYPES_HPP__
