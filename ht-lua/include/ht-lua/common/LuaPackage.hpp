#ifndef __HTLUA_LUAPACKAGE_HPP__
#define __HTLUA_LUAPACKAGE_HPP__

namespace htlua {

struct LuaPackage {
	HTAPI static void get(lua_State *L, const char *packageName);
	HTAPI static void set(lua_State *L, const char *packageName, const char *variableName);
};

} // namespace htlua

#endif // !__HTLUA_LUAPACKAGE_HPP__
