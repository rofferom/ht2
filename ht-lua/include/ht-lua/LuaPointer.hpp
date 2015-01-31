#ifndef __HTLUA_LUAPOINTER_HPP__
#define __HTLUA_LUAPOINTER_HPP__

#include <ht/Pointer.hpp>
#include <ht-lua/common/LuaUserType.hpp>

namespace htlua {

template <>
struct LuaType<ht::Pointer> : LuaUserType<ht::Pointer> {
	constexpr static const char *name = "Pointer";
};

template <>
struct LuaType<ht::PointerTable> : LuaUserType<ht::PointerTable> {
	constexpr static const char *name = "PointerTable";
};

struct LuaPointer {
	static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUAPOINTER_HPP__
