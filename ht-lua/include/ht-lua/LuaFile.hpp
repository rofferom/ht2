#ifndef __HTLUA_LUAFILE_HPP__
#define __HTLUA_LUAFILE_HPP__

#include <ht/File.hpp>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>

namespace htlua {

template <>
struct LuaType<ht::File> {
	enum { isValid = 1 };
	constexpr static const char *name = "Buffer";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<ht::File>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, ht::File *file)
	{
		LuaClass<ht::File>::forwardReference(L, file);
	}

	static void pushValue(lua_State *L, const ht::File *file)
	{
		LuaClass<ht::File>::forwardReference(L, file);
	}

	static void getValue(lua_State *L, int argIndex, ht::File *&file, bool takeOwnership)
	{
		LuaObjectParam<ht::File>::getValue(L, argIndex, file, takeOwnership);
	}

	static void getValue(lua_State *L, int argIndex, const ht::File *&file)
	{
		LuaObjectParam<ht::File>::getValue(L, argIndex, file);
	}
};

struct LuaFile {
	static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUAFILE_HPP__
