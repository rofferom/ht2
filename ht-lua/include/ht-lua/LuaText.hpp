#ifndef __HTLUA_LUATEXT_HPP__
#define __HTLUA_LUATEXT_HPP__

namespace htlua {

template <>
struct LuaType<ht::Text::BlockElement> : LuaUserType<ht::Text::BlockElement> {
	constexpr static const char *name = "ht.TextBlockElement";
};

template <>
struct LuaType<ht::Text::Block> : LuaUserType<ht::Text::Block> {
	constexpr static const char *name = "ht.TextBlock";
};

template <>
struct LuaType<ht::Text::RawBlock> : LuaUserType<ht::Text::RawBlock> {
	constexpr static const char *name = "ht.TextRawBlock";
};

template <>
struct LuaType<LuaVector<ht::Text::RawBlock *>> : LuaUserType<LuaVector<ht::Text::RawBlock *>> {
	constexpr static const char *name = "ht.TextRawBlockVector";
};

template <>
struct LuaType<ht::Text> : LuaUserType<ht::Text> {
	constexpr static const char *name = "ht.Text";
};

struct LuaText {
	HTAPI static int registerClass(lua_State *L);
	HTAPI static void printClass();
};

} // namespace htlua

#endif // !__HTLUA_LUATEXT_HPP__
