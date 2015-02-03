#ifndef __HTLUA_LUATEXT_HPP__
#define __HTLUA_LUATEXT_HPP__

extern "C" {
	#include <lua.h>
}

#include <ht/Text.hpp>
#include <ht-lua/common/LuaUserType.hpp>
#include <ht/Env.hpp>

namespace htlua {

template <>
struct LuaType<ht::Text::BlockElement> : LuaUserType<ht::Text::BlockElement> {
	constexpr static const char *name = "TextBlockElement";
};

template <>
struct LuaType<ht::Text::Block> : LuaUserType<ht::Text::Block> {
	constexpr static const char *name = "TextBlock";
};

template <>
struct LuaType<ht::Text> : LuaUserType<ht::Text> {
	constexpr static const char *name = "Text";
};

struct LuaText {
	HTAPI static int registerClass(lua_State *L);
};

} // namespace htlua

#endif // !__HTLUA_LUATEXT_HPP__
