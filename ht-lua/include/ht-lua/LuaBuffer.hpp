#ifndef __HTLUA_LUABUFFER_HPP__
#define __HTLUA_LUABUFFER_HPP__

namespace htlua {

template <>
struct LuaType<ht::Buffer> : LuaUserType<ht::Buffer> {
	constexpr static const char *name = "Buffer";
};

struct LuaBuffer {
	HTAPI static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUABUFFER_HPP__
