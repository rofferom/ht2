#ifndef __HTLUA_LUATABLE_HPP__
#define __HTLUA_LUATABLE_HPP__

#include <ht/Table.hpp>
#include <ht-lua/common/LuaUserType.hpp>

namespace htlua {

template <>
struct LuaType<ht::Table::Entry> : LuaUserType<ht::Table::Entry> {
	constexpr static const char *name = "TableEntry";
};

template <>
struct LuaType<ht::Table> : LuaUserType<ht::Table> {
	constexpr static const char *name = "Table";
};

struct LuaTable {
	static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUATABLE_HPP__
