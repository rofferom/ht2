#include <errno.h>
#include <functional>
#include <ht/Utils.hpp>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>
#include <ht-lua/common/LuaCallback.hpp>
#include <ht-lua/LuaFile.hpp>
#include <ht-lua/LuaPointer.hpp>

namespace htlua {

struct LuaTextPointerClass : LuaClass<ht::Pointer> {
	static void init()
	{
		static Method methods[] = {
			{ "getId", GetSetGenerator<uint32_t>::get(offsetof(ht::Pointer, mId)) },
			{ "setId", GetSetGenerator<uint32_t>::set(offsetof(ht::Pointer, mId)) },
			{ "getOffset", GetSetGenerator<uint32_t>::get(offsetof(ht::Pointer, mOffset)) },
			{ "setOffset", GetSetGenerator<uint32_t>::set(offsetof(ht::Pointer, mOffset)) },
			{ "getSourceAddress", GetSetGenerator<uint32_t>::get(offsetof(ht::Pointer, mSourceAddress)) },
			{ "setSourceAddress", GetSetGenerator<uint32_t>::set(offsetof(ht::Pointer, mSourceAddress)) },
			Method::empty(),
		};

		mName = LuaType<ht::Pointer>::name;
		mPackage = "ht";
		mMethods = methods;
	}
};

struct LuaPointerTableClass : LuaClass<ht::PointerTable> {
	static void init()
	{
		static Method methods[] = {
			{ "getCount", MethodGenerator<size_t(void)>::get(&ht::PointerTable::getCount) },
			{ "getPointer", MethodGenerator<ht::Pointer(size_t)>::get(&getPointerHandler) },
			{ "updateSourceAddress", MethodGenerator<int(htlua::LuaCallback<uint32_t(uint32_t)>)>::get(&updateSourceAddressHandler) },
			{ "updateOffset", MethodGenerator<int(htlua::LuaCallback<uint32_t(uint32_t)>)>::get(&updateOffsetHandler) },
			{ "write", MethodGenerator<int(ht::File, size_t, int)>::get(&writeHandler) },
			Method::empty(),
		};

		mName = LuaType<ht::PointerTable>::name;
		mPackage = "ht";
		mMethods = methods;
	}

	static int getPointerHandler(lua_State *L, ht::PointerTable *pointerTable)
	{
		ht::Pointer *pointer;
		size_t index;

		LuaType<size_t>::getValue(L, 2, index);

		pointer = pointerTable->getPointer(index);
		LuaType<ht::Pointer>::pushValue(L, pointer);
		return 1;
	}

	static int writeHandler(lua_State *L, ht::PointerTable *pointerTable)
	{
		ht::File *file;
		size_t width;
		int endianness;
		int res;

		LuaType<ht::File>::getValue(L, 2, file, false);
		LuaType<size_t>::getValue(L, 3, width);
		LuaType<int>::getValue(L, 4, endianness);

		res = pointerTable->write(file, width, (ht::PointerTable::Endianness) endianness);

		LuaType<int>::pushValue(L, res);
		return 1;
	}

	static int updateSourceAddressHandler(lua_State *L, ht::PointerTable *pointerTable)
	{
		uint32_t res;

		auto cb = htlua::LuaCallback<uint32_t(uint32_t)>::get(L, 2);
		res = pointerTable->updateSourceAddress(cb);

		LuaType<uint32_t>::pushValue(L, res);
		return 1;
	}

	static int updateOffsetHandler(lua_State *L, ht::PointerTable *pointerTable)
	{
		uint32_t res;

		auto cb = htlua::LuaCallback<uint32_t(uint32_t)>::get(L, 2);
		res = pointerTable->updateOffset(cb);

		LuaType<uint32_t>::pushValue(L, res);
		return 1;
	}
};

int LuaPointer::registerClass(lua_State *L)
{
	int res = 0;

	LuaTextPointerClass::init();
	res |= LuaTextPointerClass::registerClass(L);

	LuaPointerTableClass::init();
	res |= LuaPointerTableClass::registerClass(L);

	return res;
}

} // namespace htlua
