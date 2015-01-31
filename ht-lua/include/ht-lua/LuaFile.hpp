#ifndef __HTLUA_LUAFILE_HPP__
#define __HTLUA_LUAFILE_HPP__

#include <ht/File.hpp>
#include <ht-lua/common/LuaUserType.hpp>

namespace htlua {

template <>
struct LuaType<ht::File> : LuaUserType<ht::File> {
	constexpr static const char *name = "File";
};

struct LuaFile {
	static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUAFILE_HPP__
