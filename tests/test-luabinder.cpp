#include <stdio.h>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/LuaTable.hpp>

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

class Test {
public:
	int mA;

public:
	Test()
	{
		mA = 0;
	}

	void testVoid()
	{
		printf("testVoid\n");
	}

	int testReturnValue()
	{
		printf("testReturnValue\n");
		return 43;
	}

	void testOneArg(int i) const
	{
		printf("testOneArg : %d\n", i);
	}

	int testOneArgReturn(int i)
	{
		printf("testOneArgReturn : %d\n", i);
		return i;
	}

	int testCustomHandler(int i)
	{
		printf("testCustomHandler : %d\n", i);
		return i;
	}

	void testTwoArgs(int i, int j) const
	{
		printf("testTwoArgs : (%d, %d)\n", i, j);
	}

	std::string testStringArg(std::string s) const
	{
		printf("testStringArg : %s\n", s.c_str());
		return "Return of testStringArg";
	}

	std::string testStringArgConst(const std::string &s) const
	{
		printf("testStringArgConst : %s\n", s.c_str());
		return "Return of testStringArg";
	}
};

struct LuaTest : htlua::LuaClass<Test> {
	static void init()
	{
		static Method<Test> methods[] = {
			{ "testVoid", MethodGenerator<void(void)>::get(&Test::testVoid) },
			{ "testReturnValue", MethodGenerator<int(void)>::get(&Test::testReturnValue) },
			{ "testOneArg", MethodGenerator<void(int)>::get(&Test::testOneArg) },
			{ "testOneArgReturn", MethodGenerator<int(int)>::get(&Test::testOneArgReturn) },
			{ "customHandler", MethodGenerator<int(int)>::get(&customHandler) },
			{ "testTwoArgs", MethodGenerator<void(int, int)>::get(&Test::testTwoArgs) },
			{ "testStringArg", MethodGenerator<std::string(std::string)>::get(&Test::testStringArg) },
			{ "testStringArgConst", MethodGenerator<std::string(const std::string&)>::get(&Test::testStringArgConst) },
			{ "getA", GetSetGenerator<int>::get(offsetof(Test, mA)) },
			{ "setA", GetSetGenerator<int>::set(offsetof(Test, mA)) },
			Method<Test>::empty(),
		};

		mName = "Test";
		mMethods = methods;
	}

	static int customHandler(lua_State *L, Test *test)
	{
		int res;
		printf("customHandler\n");

		res = test->testCustomHandler(lua_tointeger(L, 2));
		lua_pushnumber(L, res);

		return 1;
	}
};

int registerLuaTest(lua_State *L)
{
	LuaTest::init();
	return LuaTest::registerClass(L);
}

int forwardTestReference(lua_State *L, Test *test)
{
	return LuaTest::forwardReference(L, test);
}

int forwardTestReference(lua_State *L, const Test *test)
{
	return LuaTest::forwardReference(L, test);
}

int main(int argc, char *argv[])
{
	lua_State * L;
	Test test;
	int e;

	if (argc != 2) {
		printf("Invalid args\n");
		printf("Usage : %s <luafile>\n", argv[0]);
		return 1;
	}

	L = luaL_newstate();
	luaL_openlibs(L);

	// Register Table binding
	htlua::LuaTable::registerClass(L);

	// Register Test binding
	registerLuaTest(L);

	forwardTestReference(L, &test);
	lua_setglobal(L, "testRef");

	forwardTestReference(L, (const Test *) &test);
	lua_setglobal(L, "testRefConst");

	e = luaL_dofile(L, argv[1]);
	if (e)
		printf("%s\n", lua_tostring(L, -1));

	lua_close(L);

	return 0;
}
