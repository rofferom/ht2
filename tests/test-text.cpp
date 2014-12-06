#include <string.h>
#include <gtest/gtest.h>
#include <ht/Log.hpp>
#include <ht/Table.hpp>
#include <ht/Text.hpp>
#include <ht/Utils.hpp>

struct TestKey {
	uint8_t mValue[10];
	size_t mSize;
};

static const TestKey key1 = {{0x10}, 1};
static const TestKey key2 = {{0x11}, 1};
static const TestKey key3 = {{0x12}, 1};
static const TestKey key4 = {{0x13}, 1};
static const TestKey key5 = {{0x14}, 1};
static const TestKey key6 = {{0x50, 0x00}, 2};
static const TestKey key7 = {{0x50, 0x01}, 2};
static const TestKey key8 = {{0x50, 0x02}, 2};

static const uint8_t encodedValue[] = {
	0x10, 0x10, 0x14, 0x13, 0x12, 0x43, 0x14, 0x13, 0x12, 0x11, 0x10, 0x44,
	0x10, 0x10, 0x50, 0x00, 0x10, 0x10, 0x43, 0x44, 0x45, 0x11, 0x11, 0x50, 0x01, 0x12, 0x12
};

TEST(Text, Encode)
{
	ht::Table table;
	ht::Text text;
	ht::Text::Block *block;
	uint8_t *rawText;
	size_t rawTextSize;
	std::list<ht::Text::Pointer *> pointerList;
	int res;

	// Fill table
	res = table.addEntry(key1.mValue, key1.mSize, U"A");
	ASSERT_EQ(res, 0);
	res = table.addEntry(key2.mValue, key2.mSize, U"B");
	ASSERT_EQ(res, 0);
	res = table.addEntry(key3.mValue, key3.mSize, U"C");
	ASSERT_EQ(res, 0);
	res = table.addEntry(key4.mValue, key4.mSize, U"D");
	ASSERT_EQ(res, 0);
	res = table.addEntry(key5.mValue, key5.mSize, U"É");
	ASSERT_EQ(res, 0);
	res = table.addEntry(key6.mValue, key6.mSize, U"[TestBlock1]");
	ASSERT_EQ(res, 0);
	res = table.addEntry(key7.mValue, key7.mSize, U"[TestBlock2]");
	ASSERT_EQ(res, 0);
	res = table.addEntry(key8.mValue, key8.mSize, U"[TestBlock3]");
	ASSERT_EQ(res, 0);

	// Fill text
	block = new ht::Text::Block();
	block->mPointerList.push_back(new ht::Text::Pointer{1, 0, 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"AAÉDC", 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x43});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"ÉDCBA", 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x44});
	text.addBlock(block);

	block = new ht::Text::Block();
	block->mPointerList.push_back(new ht::Text::Pointer{2, 0, 0});
	block->mPointerList.push_back(new ht::Text::Pointer{3, 0, 0});
	block->mPointerList.push_back(new ht::Text::Pointer{4, 0, 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"AA[TestBlock1]AA", 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x43});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x44});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x45});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"BB[TestBlock2]CC", 0});
	text.addBlock(block);

	// Try to encode text
	res = text.encode(table, &rawText, &rawTextSize, &pointerList);
	ASSERT_EQ(res, 0);
	ASSERT_EQ(rawTextSize, HT_SIZEOF_ARRAY(encodedValue));
	ASSERT_EQ(memcmp(rawText, encodedValue, HT_SIZEOF_ARRAY(encodedValue)), 0);

	// Check pointers
	ASSERT_EQ(pointerList.size(), 4);
	auto pointerIt = pointerList.begin();
	ASSERT_EQ((*pointerIt)->mId, 1);
	ASSERT_EQ((*pointerIt)->mOffset, 0);

	for (int i = 0 ; i < 3 ; i++) {
		pointerIt++;

		ASSERT_EQ((*pointerIt)->mId, 2 + i);
		ASSERT_EQ((*pointerIt)->mOffset, 12);
	}

	free(rawText);
}