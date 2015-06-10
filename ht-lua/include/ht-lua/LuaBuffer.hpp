#ifndef __HTLUA_LUABUFFER_HPP__
#define __HTLUA_LUABUFFER_HPP__

namespace htlua {

template <>
struct LuaType<ht::Buffer> : LuaUserType<ht::Buffer> {
	constexpr static const char *name = "ht.Buffer";
};

template <>
struct LuaType<LuaVector<ht::Buffer *>> : LuaUserType<LuaVector<ht::Buffer *>> {
	constexpr static const char *name = "ht.BufferVector";
};

struct LuaBuffer {
	HTAPI static int registerClass(lua_State *L);
	HTAPI static void printClass();
};

} // namespace htlua

#endif // !__HTLUA_LUABUFFER_HPP__
