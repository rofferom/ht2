#include <errno.h>
#include <functional>
#include <ht/Utils.hpp>
#include <ht-lua/LuaBuffer.hpp>

namespace htlua {

struct LuaBufferClass : LuaClass<ht::Buffer> {
	static void init()
	{
		static Method methods[] = {
			{ "getSize", MethodGenerator<size_t(void)>::get(&ht::Buffer::getSize) },
			Method::empty(),
		};

		mName = LuaType<ht::Buffer>::name;
		mPackage = "ht";
		mMethods = methods;
	}
};

int LuaBuffer::registerClass(lua_State *L)
{
	int res = 0;

	LuaBufferClass::init();
	res |= LuaBufferClass::registerClass(L);

	return res;
}

} // namespace htlua
