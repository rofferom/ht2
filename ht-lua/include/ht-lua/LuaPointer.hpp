#ifndef __HTLUA_LUAPOINTER_HPP__
#define __HTLUA_LUAPOINTER_HPP__

#include <ht/Pointer.hpp>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>

namespace htlua {

template <>
struct LuaType<ht::Pointer> {
	enum { isValid = 1 };
	constexpr static const char *name = "Pointer";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<ht::Pointer>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, ht::Pointer *pointer)
	{
		LuaClass<ht::Pointer>::forwardReference(L, pointer);
	}

	static void pushValue(lua_State *L, const ht::Pointer *pointer)
	{
		LuaClass<ht::Pointer>::forwardReference(L, pointer);
	}

	static void getValue(lua_State *L, int argIndex, ht::Pointer *&pointer, bool takeOwnership)
	{
		LuaObjectParam<ht::Pointer>::getValue(L, argIndex, pointer, takeOwnership);
	}

	static void getValue(lua_State *L, int argIndex, const ht::Pointer *&pointer)
	{
		LuaObjectParam<ht::Pointer>::getValue(L, argIndex, pointer);
	}
};

template <>
struct LuaType<ht::PointerTable> {
	enum { isValid = 1 };
	constexpr static const char *name = "PointerTable";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<ht::PointerTable>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, ht::PointerTable *pointerTable)
	{
		LuaClass<ht::PointerTable>::forwardReference(L, pointerTable);
	}

	static void pushValue(lua_State *L, const ht::PointerTable *pointerTable)
	{
		LuaClass<ht::PointerTable>::forwardReference(L, pointerTable);
	}

	static void getValue(lua_State *L, int argIndex, ht::PointerTable *&pointerTable, bool takeOwnership)
	{
		LuaObjectParam<ht::PointerTable>::getValue(L, argIndex, pointerTable, takeOwnership);
	}

	static void getValue(lua_State *L, int argIndex, const ht::PointerTable *&pointerTable)
	{
		LuaObjectParam<ht::PointerTable>::getValue(L, argIndex, pointerTable);
	}
};


struct LuaPointer {
	static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUAPOINTER_HPP__
