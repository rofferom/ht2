#include <stdio.h>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>
#include <ht-lua/common/LuaFunction.hpp>
#include <ht-lua/LuaBuffer.hpp>
#include <ht-lua/LuaFile.hpp>
#include <ht-lua/LuaPointer.hpp>
#include <ht-lua/LuaTable.hpp>
#include <ht-lua/LuaText.hpp>
#include <ht-lua-legacy/LuaTable.hpp>
#include <ht-lua-legacy/LuaText.hpp>

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

class Test2 {
public:
	int mA;

public:
	Test2()
	{
		mA = 0;
	}

	void testOtherClass(Test *test)
	{
		printf("Test2 : Got Test class with Test::mA == %d\n", test->mA);
	}
};

class Test3 {
public:
	int mA;

public:
	Test3()
	{
		mA = 0;
	}
};

namespace htlua {

template <>
struct LuaType<Test> {
	enum { isValid = 1 };
	constexpr static const char *name = "Test";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<Test>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, Test *test)
	{
		LuaClass<Test>::forwardReference(L, test);
	}

	static void getValue(lua_State *L, int argIndex, Test *&t)
	{
		LuaObjectParam<Test>::getValue(L, argIndex, t);
	}
};

} // namespace htlua


struct LuaTest : htlua::LuaClass<Test> {
	static void init()
	{
		static Method methods[] = {
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
			Method::empty(),
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

struct LuaTest2 : htlua::LuaClass<Test2> {
	static void init()
	{
		static Method methods[] = {
			{ "testOtherClass", MethodGenerator<void(Test *)>::get(&Test2::testOtherClass) },
			{ "getA", GetSetGenerator<int>::get(offsetof(Test2, mA)) },
			{ "setA", GetSetGenerator<int>::set(offsetof(Test2, mA)) },
			Method::empty(),
		};

		mName = "Test2";
		mMethods = methods;
	}
};

struct LuaTest3 : htlua::LuaClass<Test3> {
	static void init()
	{
		static Method methods[] = {
			{ "getA", GetSetGenerator<int>::get(offsetof(Test3, mA)) },
			{ "setA", GetSetGenerator<int>::set(offsetof(Test3, mA)) },
			Method::empty(),
		};

		mName = "Test3";
		mMethods = methods;
	}
};


int registerLuaTest(lua_State *L)
{
	LuaTest::init();
	LuaTest::registerClass(L);

	return 0;
}

int registerLuaTest2(lua_State *L)
{
	LuaTest2::init();
	LuaTest2::registerClass(L);

	return 0;
}

int registerLuaTest3(lua_State *L)
{
	LuaTest3::init();
	LuaTest3::registerClass(L);

	return 0;
}

int forwardTestReference(lua_State *L, Test *test)
{
	return LuaTest::forwardReference(L, test);
}

int forwardTestReference(lua_State *L, const Test *test)
{
	return LuaTest::forwardReference(L, test);
}

int testFunction1(int a)
{
	printf("%s : %d\n", __func__, a);

	return 1;
}

int testFunction2(int a, int b)
{
	printf("%s : %d %d\n", __func__, a, b);

	return 2;
}

int testFunction3(int a, int b, int c)
{
	printf("%s : %d %d %d\n", __func__, a, b, c);

	return 3;
}

struct TestFunctionBinder : htlua::LuaFunction<TestFunctionBinder> {
	static void init()
	{
		static Function functions[] = {
			{ "testFunction1", FunctionGenerator<int(int)>::get(testFunction1), },
			{ "testFunction2", FunctionGenerator<int(int, int)>::get(testFunction2), },
			{ "testFunction4", FunctionGenerator<int(int, int, int)>::get(customHandler), },
			Function::empty(),
		};

		mFunctions = functions;
	}

	static int customHandler(lua_State *L)
	{
		printf("%s\n", __func__);

		return 0;
	}
};

struct TestFunctionBinder2 : htlua::LuaFunction<TestFunctionBinder2> {
	static void init()
	{
		static Function functions[] = {
			{ "testFunction3", FunctionGenerator<int(int, int, int)>::get(testFunction3), },
			Function::empty(),
		};

		mFunctions = functions;
	}
};

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

	// Register File binding
	htlua::LuaFile::registerClass(L);

	// Register Buffer binding
	htlua::LuaBuffer::registerClass(L);

	// Register Table binding
	htlua::LuaTable::registerClass(L);

	// Register Pointer binding
	htlua::LuaPointer::registerClass(L);

	// Register Text binding
	htlua::LuaText::registerClass(L);

	// Register legacy Table binding
	htlual::LuaTable::registerFunctions(L);

	// Register legacy Text binding
	htlual::LuaText::registerFunctions(L);

	// Register Test binding
	registerLuaTest(L);
	registerLuaTest2(L);
	registerLuaTest3(L);

	// Register function binding
	TestFunctionBinder::init();
	TestFunctionBinder::registerFunction(L);

	TestFunctionBinder2::init();
	TestFunctionBinder2::registerFunction(L);

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
