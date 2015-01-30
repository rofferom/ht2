#include <ht-legacy/Text.hpp>
#include <ht-lua/LuaText.hpp>
#include <ht-lua-legacy/LuaText.hpp>

namespace {

struct LuaTextFunction : htlua::LuaFunction<LuaTextFunction> {
	static void init()
	{
		static Function functions[] = {
			{ "loadText", FunctionGenerator<int(std::string, std::string, ht::Text)>::get(loadTextHandler), },
			{ "saveText", FunctionGenerator<int(ht::Text, std::string, std::string)>::get(saveTextHandler), },
			Function::empty(),
		};

		mFunctions = functions;
		mPackage = "htl";
	}

	static int loadTextHandler(lua_State *L)
	{
		std::string textPath;
		std::string encoding;
		ht::Text *text;
		int res;

		htlua::LuaType<std::string>::getValue(L, 1, textPath);
		htlua::LuaType<std::string>::getValue(L, 2, encoding);
		htlua::LuaType<ht::Text>::getValue(L, 3, text, false);

		res = htl::loadText(textPath.c_str(), encoding.c_str(), text);
		htlua::LuaType<int>::pushValue(L, res);

		return 1;
	}

	static int saveTextHandler(lua_State *L)
	{
		const ht::Text *text;
		std::string textPath;
		std::string encoding;
		int res;

		htlua::LuaType<ht::Text>::getValue(L, 1, text);
		htlua::LuaType<std::string>::getValue(L, 2, textPath);
		htlua::LuaType<std::string>::getValue(L, 3, encoding);

		res = htl::saveText(text, textPath.c_str(), encoding.c_str());
		htlua::LuaType<int>::pushValue(L, res);

		return 1;

		return 1;
	}
};

} // anonymous namespace

namespace htlual {

int LuaText::registerFunctions(lua_State *L)
{
	LuaTextFunction::init();
	LuaTextFunction::registerFunction(L);

	return 0;
}

} // namespace htlual
