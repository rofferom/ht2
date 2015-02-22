#ifndef __HTLUA_LUAPOINTER_HPP__
#define __HTLUA_LUAPOINTER_HPP__

namespace htlua {

template <>
struct LuaType<ht::PointerEndianness> : LuaUserEnum<ht::PointerEndianness> {
	constexpr static const char *name = "ht.PointerEndianness";
};

template <>
struct LuaType<ht::Pointer> : LuaUserType<ht::Pointer> {
	constexpr static const char *name = "ht.Pointer";
};

template <>
struct LuaType<ht::PointerTable> : LuaUserType<ht::PointerTable> {
	constexpr static const char *name = "ht.PointerTable";
};

struct LuaPointer {
	HTAPI static int registerClass(lua_State *L);
	HTAPI static void printClass();
};

} // namespace htlua

#endif // !__HTLUA_LUAPOINTER_HPP__
