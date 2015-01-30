#ifndef __HTLUA_LUABUFFER_HPP__
#define __HTLUA_LUABUFFER_HPP__

#include <ht/Buffer.hpp>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>

namespace htlua {

template <>
struct LuaType<ht::Buffer> {
	enum { isValid = 1 };
	constexpr static const char *name = "Buffer";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<ht::Buffer>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, ht::Buffer *buffer)
	{
		LuaClass<ht::Buffer>::forwardReference(L, buffer);
	}

	static void pushValue(lua_State *L, const ht::Buffer *buffer)
	{
		LuaClass<ht::Buffer>::forwardReference(L, buffer);
	}

	static void getValue(lua_State *L, int argIndex, ht::Buffer *&buffer, bool takeOwnership)
	{
		LuaObjectParam<ht::Buffer>::getValue(L, argIndex, buffer, takeOwnership);
	}

	static void getValue(lua_State *L, int argIndex, const ht::Buffer *&buffer)
	{
		LuaObjectParam<ht::Buffer>::getValue(L, argIndex, buffer);
	}
};

struct LuaBuffer {
	static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUABUFFER_HPP__
