#include "ht-lua-private.hpp"

namespace htlua {

struct LuaFileClass : LuaClass<ht::File> {
	static void init()
	{
		static Method methods[] = {
			{ "open", MethodGenerator<int(std::string, std::string)>::get(&openHandler) },
			{ "close", MethodGenerator<int(void)>::get(&ht::File::close) },
			{ "writeBuffer", MethodGenerator<int(off64_t, ht::Buffer)>::get(&writeBufferHandler) },
			{ "readBuffer", MethodGenerator<int(off64_t, ht::Buffer, size_t)>::get(&readBufferHandler) },
			Method::empty(),
		};

		LuaPackage::splitFullName(LuaType<ht::File>::name, &mPackage, &mName);
		mMethods = methods;
	}

	static int openHandler(lua_State *L, ht::File *file)
	{
		std::string path;
		std::string mode;
		int res;

		LuaType<std::string>::getValue(L, 2, path);
		LuaType<std::string>::getValue(L, 3, mode);

		res = file->open(path.c_str(), mode.c_str());

		LuaType<int>::pushValue(L, res);
		return 1;
	}

	static int writeBufferHandler(lua_State *L, ht::File *file)
	{
		off64_t pos;
		ht::Buffer *buffer;
		int res;

		LuaType<off64_t>::getValue(L, 2, pos);
		LuaType<ht::Buffer>::getValue(L, 3, buffer, false);

		res = file->writeBuffer(pos, buffer);

		LuaType<int>::pushValue(L, res);
		return 1;
	}

	static int readBufferHandler(lua_State *L, ht::File *file)
	{
		off64_t pos;
		ht::Buffer *buffer;
		size_t len;
		int res;

		LuaType<off64_t>::getValue(L, 2, pos);
		LuaType<ht::Buffer>::getValue(L, 3, buffer, false);
		LuaType<size_t>::getValue(L, 4, len);

		res = file->readBuffer(pos, buffer, len);

		LuaType<int>::pushValue(L, res);
		return 1;
	}
};

int LuaFile::registerClass(lua_State *L)
{
	int res = 0;

	LuaFileClass::init();
	res |= LuaFileClass::registerClass(L);

	return res;
}

void LuaFile::printClass()
{
	LuaFileClass::printClass();
}

} // namespace htlua
