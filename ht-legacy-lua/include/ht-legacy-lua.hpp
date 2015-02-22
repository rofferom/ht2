#ifndef __HT_LUA_LEGACY_HPP__
#define __HT_LUA_LEGACY_HPP__

#include <ht-lua.hpp>

#include <ht-lua-legacy/LuaTable.hpp>
#include <ht-lua-legacy/LuaText.hpp>

namespace htlual {

HTAPI int registerComponents(lua_State *L);
HTAPI void printComponents();

} // namespace htlua

#endif // !__HT_LUA_LEGACY_HPP__
