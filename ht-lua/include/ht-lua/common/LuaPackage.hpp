#ifndef __HTLUA_LUAPACKAGE_HPP__
#define __HTLUA_LUAPACKAGE_HPP__

extern "C" {
	#include <lua.h>
}

namespace htlua {

struct LuaPackage {
	static void set(lua_State *L, const char *packageName, const char *variableName);
};

} // namespace htlua

#endif // !__HTLUA_LUAPACKAGE_HPP__
