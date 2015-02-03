#ifndef __HTLUA_LUABUFFER_HPP__
#define __HTLUA_LUABUFFER_HPP__

#include <ht/Buffer.hpp>
#include <ht-lua/common/LuaUserType.hpp>
#include <ht/Env.hpp>

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
