#ifndef __HTLUALEGACY_LUATEXT_HPP__
#define __HTLUALEGACY_LUATEXT_HPP__

#include <ht-lua/common/LuaFunction.hpp>

namespace htlual {

struct LuaText {
	static int registerFunctions(lua_State *L);
};

} // namespace htlual

#endif // !__HTLUALEGACY_LUATEXT_HPP__
