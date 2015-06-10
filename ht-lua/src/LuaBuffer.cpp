#include "ht-lua-private.hpp"

namespace htlua {

template <>
void luaObjectPredestroy(LuaObject<std::vector<ht::Buffer *>> *object)
{
	for (auto item : *(object->mInstance)) {
		delete item;
	}
}

struct LuaBufferVectorClass : LuaVector<ht::Buffer *> {
};

struct LuaBufferClass : LuaClass<ht::Buffer> {
	static void init()
	{
		static Method methods[] = {
			{ "clear", MethodGenerator<void(void)>::get(&ht::Buffer::clear) },
			{ "seek", MethodGenerator<int(size_t)>::get(&ht::Buffer::seek) },
			{ "getCapacity", MethodGenerator<size_t(void)>::get(&ht::Buffer::getCapacity) },
			{ "setCapacity", MethodGenerator<int(size_t)>::get(&ht::Buffer::setCapacity) },
			{ "getSize", MethodGenerator<size_t(void)>::get(&ht::Buffer::getSize) },
			{ "getByte", MethodGenerator<uint8_t(size_t)>::get(&ht::Buffer::getByte) },
			{ "setByte", MethodGenerator<void(size_t, uint8_t)>::get(&ht::Buffer::setByte) },
			Method::empty(),
		};

		LuaPackage::splitFullName(LuaType<ht::Buffer>::name, &mPackage, &mName);
		mMethods = methods;
	}
};

int LuaBuffer::registerClass(lua_State *L)
{
	int res = 0;

	LuaBufferClass::init();
	res |= LuaBufferClass::registerClass(L);

	LuaBufferVectorClass::init();
	res |= LuaBufferVectorClass::registerClass(L);

	return res;
}

void LuaBuffer::printClass()
{
	LuaBufferClass::printClass();
	LuaBufferVectorClass::printClass();
}

} // namespace htlua
