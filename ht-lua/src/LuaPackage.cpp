#include <stdio.h>
#include <ht-lua/common/LuaPackage.hpp>

namespace htlua {

void LuaPackage::get(lua_State *L, const char *packageName)
{
	// Get package. Create it if not exist
	lua_getglobal(L, packageName);
	if (lua_isnil(L, -1) == 1) {
		// Stack
		// -1 : nil
		// -2 : variable
		lua_pop(L, 1);

		// Stack
		// -1 : variable
		lua_newtable(L);
		lua_setglobal(L, packageName);
		lua_getglobal(L, packageName);
	}
}

void LuaPackage::set(lua_State *L, const char *packageName, const char *variableName)
{
	// Stack
	// -1 : variable
	get(L, packageName);

	// Stack
	// -1 : package
	// -2 : variable
	lua_pushstring(L, variableName);

	// Stack
	// -1 : varname
	// -2 : package
	// -3 : variable
	lua_pushvalue(L, -3);

	// Stack
	// -1 : variable
	// -2 : varname
	// -3 : package
	// -4 : variable
	lua_settable(L, -3);

	// Stack
	// -1 : package
	// -2 : variable
	lua_pop(L, 2);
}

} // namespace htlua
