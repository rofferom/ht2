#include "ht-lua-private.hpp"

namespace htlua {

int registerComponents(lua_State *L)
{
	int res = 0;

	res |= htlua::LuaFile::registerClass(L);
	res |= htlua::LuaBuffer::registerClass(L);
	res |= htlua::LuaTable::registerClass(L);
	res |= htlua::LuaPointer::registerClass(L);
	res |= htlua::LuaText::registerClass(L);

	return res;
}

void printComponents()
{
	htlua::LuaFile::printClass();
	htlua::LuaBuffer::printClass();
	htlua::LuaTable::printClass();
	htlua::LuaPointer::printClass();
	htlua::LuaText::printClass();
}

} // namespace htlua

