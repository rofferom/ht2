#ifndef __HTLUA_LUATABLE_HPP__
#define __HTLUA_LUATABLE_HPP__

namespace htlua {

template <>
struct LuaType<ht::Table::Entry> : LuaUserType<ht::Table::Entry> {
	constexpr static const char *name = "ht.TableEntry";
};

template <>
struct LuaType<ht::Table> : LuaUserType<ht::Table> {
	constexpr static const char *name = "ht.Table";
};

struct LuaTable {
	HTAPI static int registerClass(lua_State *L);
	HTAPI static void printClass();
};

} // namespace htlua

#endif // !__HTLUA_LUATABLE_HPP__
