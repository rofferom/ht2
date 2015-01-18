#ifndef __HTLUA_LUATEXT_HPP__
#define __HTLUA_LUATEXT_HPP__

extern "C" {
	#include <lua.h>
}

#include <ht/Text.hpp>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>

namespace htlua {

template <>
struct LuaType<ht::Text> {
	enum { isValid = 1 };
	constexpr static const char *name = "Table";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<ht::Text>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, ht::Text *pointer)
	{
		LuaClass<ht::Text>::forwardReference(L, pointer);
	}

	static void pushValue(lua_State *L, const ht::Text *pointer)
	{
		LuaClass<ht::Text>::forwardReference(L, pointer);
	}

	static void getValue(lua_State *L, int argIndex, ht::Text *&pointer, bool takeOwnership)
	{
		LuaObjectParam<ht::Text>::getValue(L, argIndex, pointer, takeOwnership);
	}

	static void getValue(lua_State *L, int argIndex, const ht::Text *&pointer)
	{
		LuaObjectParam<ht::Text>::getValue(L, argIndex, pointer);
	}
};

struct LuaText {
	static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUATEXT_HPP__
