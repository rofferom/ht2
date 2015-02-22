#include "ht-legacy-lua-private.hpp"

namespace {

struct LuaTableFunction : htlua::LuaFunction<LuaTableFunction> {
	static void init()
	{
		static Function functions[] = {
			{ "loadTable", FunctionGenerator<int(std::string, std::string, ht::Table)>::get(loadTableHandler), },
			Function::empty(),
		};

		mFunctions = functions;
		mPackage = "htl";
	}

	static int loadTableHandler(lua_State *L)
	{
		std::string tablePath;
		std::string encoding;
		ht::Table *table;
		int res;

		htlua::LuaType<std::string>::getValue(L, 1, tablePath);
		htlua::LuaType<std::string>::getValue(L, 2, encoding);
		htlua::LuaType<ht::Table>::getValue(L, 3, table, false);

		res = htl::loadTable(tablePath.c_str(), encoding.c_str(), table);
		htlua::LuaType<int>::pushValue(L, res);

		return 1;
	}
};

} // anonymous namespace

namespace htlual {

int LuaTable::registerFunctions(lua_State *L)
{
	LuaTableFunction::init();
	LuaTableFunction::registerFunction(L);

	return 0;
}

void LuaTable::printFunctions()
{
	LuaTableFunction::printFunction();
}

} // namespace htlual
