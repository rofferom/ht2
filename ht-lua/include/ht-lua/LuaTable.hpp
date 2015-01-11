#ifndef __HTLUA_LUATABLE_HPP__
#define __HTLUA_LUATABLE_HPP__

#include <ht/Table.hpp>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>

namespace htlua {

template <>
struct LuaType<ht::Table> {
	enum { isValid = 1 };
	constexpr static const char *name = "Table";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<ht::Table>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, ht::Table *pointer)
	{
		LuaClass<ht::Table>::forwardReference(L, pointer);
	}

	static void pushValue(lua_State *L, const ht::Table *pointer)
	{
		LuaClass<ht::Table>::forwardReference(L, pointer);
	}

	static void getValue(lua_State *L, int argIndex, ht::Table *&pointer, bool takeOwnership)
	{
		LuaObjectParam<ht::Table>::getValue(L, argIndex, pointer, takeOwnership);
	}

	static void getValue(lua_State *L, int argIndex, const ht::Table *&pointer)
	{
		LuaObjectParam<ht::Table>::getValue(L, argIndex, pointer);
	}
};

struct LuaTable {
	static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUATABLE_HPP__
