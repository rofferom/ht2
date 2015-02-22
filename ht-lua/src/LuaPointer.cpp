#include "ht-lua-private.hpp"

namespace htlua {

struct LuaPointerTableEndiannessEnum : htlua::LuaEnum<ht::PointerEndianness> {
	static void init()
	{
		static Enum enums[] = {
			{"Little", ht::PointerEndianness::Little},
			{"Big", ht::PointerEndianness::Big},
			Enum::empty(),
		};

		LuaPackage::splitFullName(LuaType<ht::PointerEndianness>::name, &mPackage, &mName);
		mEnums = enums;
	}
};

struct LuaPointerClass : LuaClass<ht::Pointer> {
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

		LuaPackage::splitFullName(LuaType<ht::Pointer>::name, &mPackage, &mName);
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
			{ "read", MethodGenerator<int(ht::File, off64_t, uint32_t, size_t, ht::PointerEndianness)>::get(&readHandler) },
			Method::empty(),
		};

		LuaPackage::splitFullName(LuaType<ht::PointerTable>::name, &mPackage, &mName);
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
		ht::PointerEndianness endianness;
		int res;

		LuaType<ht::File>::getValue(L, 2, file, false);
		LuaType<size_t>::getValue(L, 3, width);
		LuaType<ht::PointerEndianness>::getValue(L, 4, endianness);

		res = pointerTable->write(file, width, (ht::PointerEndianness) endianness);

		LuaType<int>::pushValue(L, res);
		return 1;
	}

	static int readHandler(lua_State *L, ht::PointerTable *pointerTable)
	{
		ht::File *file;
		off64_t pos;
		uint32_t count;
		size_t width;
		ht::PointerEndianness endianness;
		int res;

		LuaType<ht::File>::getValue(L, 2, file, false);
		LuaType<off64_t>::getValue(L, 3, pos);
		LuaType<uint32_t>::getValue(L, 4, count);
		LuaType<size_t>::getValue(L, 5, width);
		LuaType<ht::PointerEndianness>::getValue(L, 6, endianness);

		res = pointerTable->read(file, pos, count, width, (ht::PointerEndianness) endianness);

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

	LuaPointerTableEndiannessEnum::init();
	res |= LuaPointerTableEndiannessEnum::registerEnum(L);

	LuaPointerClass::init();
	res |= LuaPointerClass::registerClass(L);

	LuaPointerTableClass::init();
	res |= LuaPointerTableClass::registerClass(L);

	return res;
}

void LuaPointer::printClass()
{
	LuaPointerTableEndiannessEnum::printEnum();
	LuaPointerClass::printClass();
	LuaPointerTableClass::printClass();
}

} // namespace htlua
