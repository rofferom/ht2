#include "ht-lua-private.hpp"

namespace {

struct LuaTextFunction : htlua::LuaFunction<LuaTextFunction> {
	static void init()
	{
		static Function functions[] = {
			{ "encodeString", FunctionGenerator<int(std::string, ht::Table, ht::Buffer)>::get(encodeStringHandler), },
			{ "encodeBlock", FunctionGenerator<int(ht::Text::Block, ht::Table, ht::Buffer)>::get(encodeBlockHandler), },
			{ "decodeBuffer", FunctionGenerator<int(ht::Buffer, ht::Table, ht::Text::Block)>::get(decodeBufferHandler), },
			{ "splitText", FunctionGenerator<int(ht::Buffer, ht::PointerTable, htlua::LuaVector<ht::Text::RawBlock *>)>::get(splitTextHandler), },
			{ "splitRawText", FunctionGenerator<int(ht::Buffer, uint8_t, htlua::LuaVector<ht::Buffer *>)>::get(splitRawTextHandler), },
			Function::empty(),
		};

		mFunctions = functions;
		mPackage = "ht.text";
	}

	static int encodeStringHandler(lua_State *L)
	{
		std::u32string s;
		ht::Table *table;
		ht::Buffer *buffer;
		int res;

		htlua::LuaType<std::u32string>::getValue(L, 1, s);
		htlua::LuaType<ht::Table>::getValue(L, 2, table, false);
		htlua::LuaType<ht::Buffer>::getValue(L, 3, buffer, false);

		res = ht::Text::encodeString(s, *table, buffer);
		htlua::LuaType<int>::pushValue(L, res);

		return 1;
	}

	static int encodeBlockHandler(lua_State *L)
	{
		ht::Text::Block *block;
		ht::Table *table;
		ht::Buffer *buffer;
		int res;

		htlua::LuaType<ht::Text::Block>::getValue(L, 1, block, false);
		htlua::LuaType<ht::Table>::getValue(L, 2, table, false);
		htlua::LuaType<ht::Buffer>::getValue(L, 3, buffer, false);

		res = ht::Text::encodeBlock(block, *table, buffer);
		htlua::LuaType<int>::pushValue(L, res);

		return 1;
	}

	static int decodeBufferHandler(lua_State *L)
	{
		ht::Buffer *buffer;
		ht::Table *table;
		ht::Text::Block *block;
		int res;

		htlua::LuaType<ht::Buffer>::getValue(L, 1, buffer, false);
		htlua::LuaType<ht::Table>::getValue(L, 2, table, false);
		htlua::LuaType<ht::Text::Block>::getValue(L, 3, block, false);

		res = ht::Text::decodeBuffer(buffer->getData(), buffer->getSize(), *table, block);
		htlua::LuaType<int>::pushValue(L, res);

		return 1;
	}

	static int splitTextHandler(lua_State *L)
	{
		ht::Buffer *buffer;
		ht::PointerTable *pointerList;
		std::vector<ht::Text::RawBlock *> *blockList;
		int res;

		htlua::LuaType<ht::Buffer>::getValue(L, 1, buffer, false);
		htlua::LuaType<ht::PointerTable>::getValue(L, 2, pointerList, false);
		htlua::LuaType<htlua::LuaVector<ht::Text::RawBlock *>>::getValue(L, 3, blockList, false);

		res = ht::Text::splitText(*buffer, *pointerList, blockList);
		htlua::LuaType<int>::pushValue(L, res);

		return 1;
	}

	static int splitRawTextHandler(lua_State *L)
	{
		ht::Buffer *buffer;
		uint8_t separator;
		std::vector<ht::Buffer *> *splittedBuffer;;
		int res;

		htlua::LuaType<ht::Buffer>::getValue(L, 1, buffer, false);
		htlua::LuaType<uint8_t>::getValue(L, 2, separator);
		htlua::LuaType<htlua::LuaVector<ht::Buffer *>>::getValue(L, 3, splittedBuffer, false);

		res = ht::Text::splitRawText(*buffer, separator, splittedBuffer);
		htlua::LuaType<int>::pushValue(L, res);

		return 1;
	}
};

} // anonymous namespace

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

		LuaPackage::splitFullName(LuaType<ht::Text::BlockElement>::name, &mPackage, &mName);
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

struct LuaTextBlockClass : LuaClass<ht::Text::Block> {
	static void init()
	{
		static Method methods[] = {
			{ "getPointerCount", MethodGenerator<size_t(void)>::get(getPointerCount) },
			{ "getPointer", MethodGenerator<ht::Pointer *(size_t)>::get(getPointer) },
			{ "addPointer", MethodGenerator<void(ht::Pointer *)>::get(addPointer) },
			{ "addRawBlockPointers", MethodGenerator<void(ht::Text::RawBlock *)>::get(addRawBlockPointers) },
			{ "getElementCount", MethodGenerator<size_t(void)>::get(getElementCount) },
			{ "getElement", MethodGenerator<ht::Text::BlockElement *(size_t)>::get(getElement) },
			{ "addElement", MethodGenerator<void(ht::Text::BlockElement *)>::get(addElement) },
			Method::empty(),
		};

		LuaPackage::splitFullName(LuaType<ht::Text::Block>::name, &mPackage, &mName);
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

	static int addRawBlockPointers(lua_State *L, ht::Text::Block *block)
	{
		ht::Text::RawBlock *rawBlock;

		LuaType<ht::Text::RawBlock>::getValue(L, 2, rawBlock, false);

		block->mPointerList = rawBlock->mPointerList;

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

struct LuaTextRawBlockClass : LuaClass<ht::Text::RawBlock> {
	static void init()
	{
		static Method methods[] = {
			{ "getPointerCount", MethodGenerator<size_t(void)>::get(getPointerCount) },
			{ "getPointer", MethodGenerator<ht::Pointer *(size_t)>::get(getPointer) },
			{ "addPointer", MethodGenerator<void(ht::Pointer *)>::get(addPointer) },
			{ "getBuffer", MethodGenerator<ht::Buffer *(void)>::get(getBuffer) },
			Method::empty(),
		};

		LuaPackage::splitFullName(LuaType<ht::Text::RawBlock>::name, &mPackage, &mName);
		mMethods = methods;
	}

	static int getPointerCount(lua_State *L, ht::Text::RawBlock *block)
	{
		LuaType<size_t>::pushValue(L, block->mPointerList.size());

		return 1;
	}

	static int getPointer(lua_State *L, ht::Text::RawBlock *block)
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

	static int addPointer(lua_State *L, ht::Text::RawBlock *block)
	{
		ht::Pointer *pointer;

		LuaType<ht::Pointer>::getValue(L, 2, pointer, true);

		block->mPointerList.push_back(*pointer);

		return 0;
	}

	static int getBuffer(lua_State *L, ht::Text::RawBlock *block)
	{
		LuaType<ht::Buffer>::pushValue(L, &block->mBuffer);

		return 1;
	}
};

template <>
void luaObjectPredestroy(LuaObject<std::vector<ht::Text::RawBlock *>> *object)
{
	for (auto item : *(object->mInstance)) {
		delete item;
	}
}

struct LuaTextRawBlockVectorClass : LuaVector<ht::Text::RawBlock *> {
};

struct LuaTextClass : LuaClass<ht::Text> {
	static void init()
	{
		static Method methods[] = {
			{ "getBlockCount", MethodGenerator<size_t(void)>::get(&ht::Text::getBlockCount) },
			{ "addBlock", MethodGenerator<int(ht::Text::Block)>::get(addBlockHandler) },
			{ "encode", MethodGenerator<int(ht::Table, ht::Buffer, ht::PointerTable)>::get(encodeHandler, true) },
			{ "decode", MethodGenerator<int(ht::Buffer, ht::Table, ht::PointerTable)>::get(decodeHandler, true) },
			Method::empty(),
		};

		LuaPackage::splitFullName(LuaType<ht::Text>::name, &mPackage, &mName);
		mMethods = methods;
	}

	static int addBlockHandler(lua_State *L, ht::Text *text)
	{
		ht::Text::Block *block;
		int res;

		LuaType<ht::Text::Block>::getValue(L, 2, block, true);

		res = text->addBlock(block);
		LuaType<int>::pushValue(L, res);

		return 1;
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

	LuaTextFunction::init();
	res |= LuaTextFunction::registerFunction(L);

	LuaTextBlockElementClass::init();
	res |= LuaTextBlockElementClass::registerClass(L);

	LuaTextBlockClass::init();
	res |= LuaTextBlockClass::registerClass(L);

	LuaTextRawBlockClass::init();
	res |= LuaTextRawBlockClass::registerClass(L);

	LuaTextRawBlockVectorClass::init();
	res |= LuaTextRawBlockVectorClass::registerClass(L);

	LuaTextClass::init();
	res |= LuaTextClass::registerClass(L);

	return res;
}

void LuaText::printClass()
{
	LuaTextFunction::printFunction();
	LuaTextBlockElementClass::printClass();
	LuaTextBlockClass::printClass();
	LuaTextRawBlockClass::printClass();
	LuaTextRawBlockVectorClass::printClass();
	LuaTextClass::printClass();
}

} // namespace htlua
