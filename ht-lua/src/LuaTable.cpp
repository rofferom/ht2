#include "ht-lua-private.hpp"

namespace htlua {

struct LuaTableEntryClass : LuaClass<ht::Table::Entry> {
	static void init()
	{
		static Method methods[] = {
			{ "getKey", MethodGenerator<std::u32string(void)>::get(&getKeyHandler, true) },
			{ "getValue", GetSetGenerator<std::u32string>::get(offsetof(ht::Table::Entry, mValue)) },
			Method::empty(),
		};

		mName = LuaType<ht::Table::Entry>::name;
		mPackage = "ht";
		mMethods = methods;
	}

	static int getKeyHandler(lua_State *L, ht::Table::Entry *entry)
	{
		std::u32string strkey;

		ht::encodeKey(&entry->mKey, &strkey);

		LuaType<std::u32string>::pushValue(L, strkey);

		return 1;
	}
};

static LuaTableEntryClass luaTableEntryClass;

struct LuaTableClass : LuaClass<ht::Table> {
	static void init()
	{
		static Method methods[] = {
			{ "addEntry", MethodGenerator<int(std::string, std::string)>::get(&addEntryHandler) },
			{ "getEntryCount", MethodGenerator<size_t(void)>::get(&ht::Table::getEntryCount) },
			{ "findFromValue", MethodGenerator<ht::Table::Entry *(std::string)>::get(&findFromValueHandler) },
			{ "findFromKey", MethodGenerator<ht::Table::Entry *(std::string)>::get(&findFromKeyHandler) },
			{ "getMaxKeySize", MethodGenerator<size_t(void)>::get(&ht::Table::getMaxKeySize) },
			{ "getMaxValueSize", MethodGenerator<size_t(void)>::get(&ht::Table::getMaxValueSize) },
			Method::empty(),
		};

		mName = LuaType<ht::Table>::name;
		mPackage = "ht";
		mMethods = methods;
	}

	static int addEntryHandler(lua_State *L, ht::Table *table)
	{
		std::u32string key;
		std::u32string value;
		uint8_t *parsedKey;
		size_t keySize;
		int res;

		LuaType<std::u32string>::getValue(L, 2, key);
		LuaType<std::u32string>::getValue(L, 3, value);

		res = ht::parseKey(key.c_str(), key.size(), &parsedKey, &keySize);
		if (res < 0) {
			return luaL_argerror(L, 2, "Invalid format");
		}

		res = table->addEntry(parsedKey, keySize, value);
		free(parsedKey);

		LuaType<int>::pushValue(L, res);
		return 1;
	}

	static int findFromValueHandler(lua_State *L, ht::Table *table)
	{
		std::u32string value;
		const ht::Table::Entry *entry;

		LuaType<std::u32string>::getValue(L, 2, value);

		entry = table->findFromValue(value);
		if (entry == NULL) {
			lua_pushnil(L);
		} else {
			luaTableEntryClass.forwardReference(L, entry);
		}

		return 1;
	}

	static int findFromKeyHandler(lua_State *L, ht::Table *table)
	{
		std::u32string key;
		const ht::Table::Entry *entry;
		uint8_t *parsedKey;
		size_t keySize;
		int res;

		LuaType<std::u32string>::getValue(L, 2, key);

		res = ht::parseKey(key.c_str(), key.size(), &parsedKey, &keySize);
		if (res < 0) {
			return luaL_argerror(L, 2, "Invalid format");
		}

		entry = table->findFromKey(parsedKey, keySize);
		if (entry == NULL) {
			lua_pushnil(L);
		} else {
			luaTableEntryClass.forwardReference(L, entry);
		}

		free(parsedKey);

		return 1;
	}
};

int LuaTable::registerClass(lua_State *L)
{
	int res = 0;

	LuaTableEntryClass::init();
	res |= LuaTableEntryClass::registerClass(L);

	LuaTableClass::init();
	res |= LuaTableClass::registerClass(L);

	return res;
}

} // namespace htlua
