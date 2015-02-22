#ifndef __HTLUA_LUAFILE_HPP__
#define __HTLUA_LUAFILE_HPP__

namespace htlua {

template <>
struct LuaType<ht::File> : LuaUserType<ht::File> {
	constexpr static const char *name = "ht.File";
};

struct LuaFile {
	HTAPI static int registerClass(lua_State *L);
	HTAPI static void printClass();
};

} // namespace htlua

#endif // !__HTLUA_LUAFILE_HPP__
