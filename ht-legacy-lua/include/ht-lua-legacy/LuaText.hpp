#ifndef __HTLUALEGACY_LUATEXT_HPP__
#define __HTLUALEGACY_LUATEXT_HPP__

namespace htlual {

struct LuaText {
	HTAPI static int registerFunctions(lua_State *L);
	HTAPI static void printFunctions();
};

} // namespace htlual

#endif // !__HTLUALEGACY_LUATEXT_HPP__
