#ifndef __HTLUALEGACY_LUATABLE_HPP__
#define __HTLUALEGACY_LUATABLE_HPP__

namespace htlual {

struct LuaTable {
	HTAPI static int registerFunctions(lua_State *L);
	HTAPI static void printFunctions();
};

} // namespace htlual

#endif // !__HTLUALEGACY_LUATABLE_HPP__
