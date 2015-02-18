#include "ht-legacy-lua-private.hpp"

namespace htlual {

int registerComponents(lua_State *L)
{
	int res = 0;

	res |= htlual::LuaTable::registerFunctions(L);
	res |= htlual::LuaText::registerFunctions(L);

	return res;
}

} // namespace htlual

