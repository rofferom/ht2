#include <errno.h>
#include <functional>
#include <ht/Utils.hpp>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>
#include <ht-lua/LuaBuffer.hpp>
#include <ht-lua/LuaTable.hpp>
#include <ht-lua/LuaPointer.hpp>
#include <ht-lua/LuaText.hpp>

namespace htlua {

struct LuaTextBlockElementClass : LuaClass<ht::Text::BlockElement> {
	static void init()
	{
		static Method methods[] = {
			{ "getType", MethodGenerator<std::string(void)>::get(getType, true) },
			{ "isRawByte", MethodGenerator<bool(void)>::get(isRawByte, true) },
			{ "getRawByte", MethodGenerator<uint8_t(void)>::get(getRawByte, true) },
			{ "setRawByte", MethodGenerator<void(uint8_t)>::get(setRawByte) },
			{ "isText", MethodGenerator<bool(void)>::get(isText, true) },
			{ "getText", MethodGenerator<std::u32string(void)>::get(getText, true) },
			{ "setText", MethodGenerator<void(std::u32string)>::get(setText) },
			Method::empty(),
		};

		mName = "TextBlockElement";
		mPackage = "ht";
		mMethods = methods;
	}

	static int getType(lua_State *L, ht::Text::BlockElement *blockElem)
	{
		switch (blockElem->mType) {
		case ht::Text::BlockElement::Type::RawByte:
			LuaType<std::string>::pushValue(L, "RawByte");
			break;

		case ht::Text::BlockElement::Type::Text:
			LuaType<std::string>::pushValue(L, "Text");
			break;

		default:
			LuaType<std::string>::pushValue(L, "None");
			break;
		}

		return 1;
	}

	static int isRawByte(lua_State *L, ht::Text::BlockElement *blockElem)
	{
		if (blockElem->mType == ht::Text::BlockElement::Type::RawByte) {
			LuaType<bool>::pushValue(L, true);
		} else {
			LuaType<bool>::pushValue(L, false);
		}

		return 1;
	}

	static int getRawByte(lua_State *L, ht::Text::BlockElement *blockElem)
	{
		if (blockElem->mType == ht::Text::BlockElement::Type::RawByte) {
			LuaType<uint8_t>::pushValue(L, blockElem->mRawByte);
		} else {
			lua_pushnil(L);
		}

		return 1;
	}

	static int setRawByte(lua_State *L, ht::Text::BlockElement *blockElem)
	{
		blockElem->mType = ht::Text::BlockElement::Type::RawByte;
		LuaType<uint8_t>::getValue(L, 2, blockElem->mRawByte);

		return 0;
	}

	static int isText(lua_State *L, ht::Text::BlockElement *blockElem)
	{
		if (blockElem->mType == ht::Text::BlockElement::Type::Text) {
			LuaType<bool>::pushValue(L, true);
		} else {
			LuaType<bool>::pushValue(L, false);
		}

		return 1;
	}

	static int getText(lua_State *L, ht::Text::BlockElement *blockElem)
	{
		if (blockElem->mType == ht::Text::BlockElement::Type::Text) {
			LuaType<std::u32string>::pushValue(L, blockElem->mTextContent);
		} else {
			lua_pushnil(L);
		}

		return 1;
	}

	static int setText(lua_State *L, ht::Text::BlockElement *blockElem)
	{
		blockElem->mType = ht::Text::BlockElement::Type::Text;
		LuaType<std::u32string>::getValue(L, 2, blockElem->mTextContent);

		return 0;
	}
};

template <>
struct LuaType<ht::Text::BlockElement> {
	enum { isValid = 1 };
	constexpr static const char *name = "TextBlockElement";

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<ht::Text::BlockElement>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, ht::Text::BlockElement *blockElement)
	{
		LuaClass<ht::Text::BlockElement>::forwardReference(L, blockElement);
	}

	static void pushValue(lua_State *L, const ht::Text::BlockElement *blockElement)
	{
		LuaClass<ht::Text::BlockElement>::forwardReference(L, blockElement);
	}

	static void getValue(lua_State *L, int argIndex, ht::Text::BlockElement *&blockElement, bool takeOwnership = true)
	{
		LuaObjectParam<ht::Text::BlockElement>::getValue(L, argIndex, blockElement, takeOwnership);
	}
};

struct LuaTextBlockClass : LuaClass<ht::Text::Block> {
	static void init()
	{
		static Method methods[] = {
			{ "getPointerCount", MethodGenerator<size_t(void)>::get(getPointerCount) },
			{ "getPointer", MethodGenerator<ht::Pointer *(size_t)>::get(getPointer) },
			{ "addPointer", MethodGenerator<void(ht::Pointer *)>::get(addPointer) },
			{ "getElementCount", MethodGenerator<size_t(void)>::get(getElementCount) },
			{ "getElement", MethodGenerator<ht::Text::BlockElement *(size_t)>::get(getElement) },
			{ "addElement", MethodGenerator<void(ht::Text::BlockElement *)>::get(addElement) },
			Method::empty(),
		};

		mName = "TextBlock";
		mPackage = "ht";
		mMethods = methods;
	}

	static int getPointerCount(lua_State *L, ht::Text::Block *block)
	{
		LuaType<size_t>::pushValue(L, block->mPointerList.size());

		return 1;
	}

	static int getPointer(lua_State *L, ht::Text::Block *block)
	{
		size_t index;

		LuaType<size_t>::getValue(L, 2, index);

		if (index < block->mPointerList.size()) {
			LuaType<ht::Pointer>::pushValue(L, &block->mPointerList[index]);
		} else {
			lua_pushnil(L);
		}

		return 1;
	}

	static int addPointer(lua_State *L, ht::Text::Block *block)
	{
		ht::Pointer *pointer;

		LuaType<ht::Pointer>::getValue(L, 2, pointer, true);

		block->mPointerList.push_back(*pointer);

		return 0;
	}

	static int getElementCount(lua_State *L, ht::Text::Block *block)
	{
		LuaType<size_t>::pushValue(L, block->mElementList.size());

		return 1;
	}

	static int getElement(lua_State *L, ht::Text::Block *block)
	{
		size_t index;

		LuaType<size_t>::getValue(L, 2, index);

		if (index < block->mElementList.size()) {
			LuaType<ht::Text::BlockElement>::pushValue(L, block->mElementList[index]);
		} else {
			lua_pushnil(L);
		}

		return 1;
	}

	static int addElement(lua_State *L, ht::Text::Block *block)
	{
		ht::Text::BlockElement *element;

		LuaType<ht::Text::BlockElement>::getValue(L, 2, element, true);

		block->mElementList.push_back(element);

		return 0;
	}
};

struct LuaTextClass : LuaClass<ht::Text> {
	static void init()
	{
		static Method methods[] = {
			{ "getBlockCount", MethodGenerator<size_t(void)>::get(&ht::Text::getBlockCount) },
			{ "encode", MethodGenerator<int(ht::Table, ht::Buffer, ht::PointerTable)>::get(encodeHandler, true) },
			{ "decode", MethodGenerator<int(ht::Buffer, ht::Table, ht::PointerTable)>::get(decodeHandler, true) },
			Method::empty(),
		};

		mName = "Text";
		mPackage = "ht";
		mMethods = methods;
	}

	static int encodeHandler(lua_State *L, ht::Text *text)
	{
		const ht::Table *table;
		ht::Buffer *buffer;
		ht::PointerTable *pointerList;
		int res;

		LuaType<ht::Table>::getValue(L, 2, table);
		LuaType<ht::Buffer>::getValue(L, 3, buffer, false);
		LuaType<ht::PointerTable>::getValue(L, 4, pointerList, false);

		res = text->encode(*table, buffer, pointerList);

		LuaType<int>::pushValue(L, res);
		return 1;
	}

	static int decodeHandler(lua_State *L, ht::Text *text)
	{
		const ht::Buffer *buffer;
		const ht::Table *table;
		const ht::PointerTable *pointerList;
		int res;

		LuaType<ht::Buffer>::getValue(L, 2, buffer);
		LuaType<ht::Table>::getValue(L, 3, table);
		LuaType<ht::PointerTable>::getValue(L, 4, pointerList);

		res = text->decode(*buffer, *table, *pointerList);

		LuaType<int>::pushValue(L, res);
		return 1;
	}
};

int LuaText::registerClass(lua_State *L)
{
	int res = 0;

	LuaTextBlockElementClass::init();
	res |= LuaTextBlockElementClass::registerClass(L);

	LuaTextBlockClass::init();
	res |= LuaTextBlockClass::registerClass(L);

	LuaTextClass::init();
	res |= LuaTextClass::registerClass(L);

	return res;
}

} // namespace htlua
