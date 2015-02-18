#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ht-lua/common/LuaPackage.hpp>

namespace {

void getRootPackage(lua_State *L, const char *packageName)
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

	// Stack
	// -1 : package
}

void getSubPackage(lua_State *L, const char *packageName)
{
	// Stack
	// -1 : parentPackage
	lua_pushstring(L, packageName);

	// Stack
	// -1 : packageName
	// -2 : parentPackage
	lua_gettable(L, -2);

	// Stack
	// -1 : package or nil
	// -2 : parentPackage
	if (lua_isnil(L, -1) == 1) {
		// Stack
		// -1 : nil
		// -2 : parentPackage
		lua_pop(L, 1);

		// Stack
		// -1 : parentPackage
		lua_newtable(L);

		// Stack
		// -1 : package
		// -2 : parentPackage
		lua_pushstring(L, packageName);

		// Stack
		// -1 : packageName
		// -2 : package
		// -3 : parentPackage
		lua_pushvalue(L, -2);

		// Stack
		// -1 : package
		// -2 : packageName
		// -3 : package
		// -4 : parentPackage
		lua_settable(L, -4);
	}

	// Stack
	// -1 : package
	// -2 : parentPackage
	lua_remove(L, -2);

	// Stack
	// -1 : package
}

} // anonymous namespace

namespace htlua {

void LuaPackage::get(lua_State *L, const char *packageName)
{
	char *localPackageName;
	char *packageBegin;
	char *packageEnd;
	bool lastPackage;
	bool rootPackage;

	localPackageName = strdup(packageName);
	if (localPackageName == NULL) {
		return;
	}

	rootPackage = true;
	packageBegin = localPackageName;
	do {
		packageEnd = strchr(packageBegin, '.');
		if (packageEnd == NULL) {
			lastPackage = true;
		} else {
			*packageEnd = '\0';
			lastPackage = false;
		}

		if (rootPackage == true) {
			getRootPackage(L, packageBegin);
			rootPackage = false;
		} else {
			getSubPackage(L, packageBegin);
		}

		if (lastPackage == false) {
			packageBegin = packageEnd + 1;
		}
	} while (lastPackage == false);

	free(localPackageName);
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
