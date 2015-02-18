#include <gtest/gtest.h>
#include <ht-legacy.hpp>

TEST(TextLegacy, Save)
{
	ht::Text text;
	ht::Text::Block *block;
	int res;

	// Fill text
	block = new ht::Text::Block();
	block->mPointerList.push_back(ht::Pointer{1, 0, 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"AAÉDC", 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x43});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"ÉDCBA<PT000A>\n", 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x44});
	text.addBlock(block);

	block = new ht::Text::Block();
	block->mPointerList.push_back(ht::Pointer{2, 0, 0});
	block->mPointerList.push_back(ht::Pointer{3, 0, 0});
	block->mPointerList.push_back(ht::Pointer{4, 0, 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"AA[TestBlock1]AA", 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x43});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x44});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x4D});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"BB[TestBlock2]CC", 0});
	text.addBlock(block);

	// Try to save text
	res = htl::saveText(&text, "test-legacytext-save-generated.txt", "UTF-8");
	ASSERT_EQ(res, 0);

	res = ht::compareFiles("test-legacytext-save.txt", "test-legacytext-save-generated.txt");
	ASSERT_EQ(res, 0);
}

TEST(TextLegacy, Load)
{
	ht::Text text;
	int res;

	res = htl::loadText("test-legacytext-save.txt", "UTF-8", &text);
	ASSERT_EQ(res, 0);

	// Test block 0
	ASSERT_EQ(text.getBlock(0)->mPointerList.size(), 1);
	auto pointerIt = text.getBlock(0)->mPointerList.begin();
	ASSERT_EQ(pointerIt->mId, 1);

	ASSERT_EQ(text.getBlock(0)->mElementList.size(), 4);

	auto elementIt = text.getBlock(0)->mElementList.begin();
	ASSERT_EQ((*elementIt)->mType, ht::Text::BlockElement::Type::Text);
	ASSERT_TRUE((*elementIt)->mTextContent == U"AAÉDC");

	elementIt++;
	ASSERT_EQ((*elementIt)->mType, ht::Text::BlockElement::Type::RawByte);
	ASSERT_EQ((*elementIt)->mRawByte, 0x43);

	elementIt++;
	ASSERT_EQ((*elementIt)->mType, ht::Text::BlockElement::Type::Text);
	ASSERT_TRUE((*elementIt)->mTextContent == U"ÉDCBA<PT000A>\n");

	elementIt++;
	ASSERT_EQ((*elementIt)->mType, ht::Text::BlockElement::Type::RawByte);
	ASSERT_EQ((*elementIt)->mRawByte, 0x44);

	// Test block 1
	ASSERT_EQ(text.getBlock(1)->mPointerList.size(), 3);
	pointerIt = text.getBlock(1)->mPointerList.begin();
	ASSERT_EQ(pointerIt->mId, 2);
	pointerIt++;
	ASSERT_EQ(pointerIt->mId, 3);
	pointerIt++;
	ASSERT_EQ(pointerIt->mId, 4);

	ASSERT_EQ(text.getBlock(1)->mElementList.size(), 5);

	elementIt = text.getBlock(1)->mElementList.begin();
	ASSERT_EQ((*elementIt)->mType, ht::Text::BlockElement::Type::Text);
	ASSERT_TRUE((*elementIt)->mTextContent == U"AA[TestBlock1]AA");

	elementIt++;
	ASSERT_EQ((*elementIt)->mType, ht::Text::BlockElement::Type::RawByte);
	ASSERT_EQ((*elementIt)->mRawByte, 0x43);

	elementIt++;
	ASSERT_EQ((*elementIt)->mType, ht::Text::BlockElement::Type::RawByte);
	ASSERT_EQ((*elementIt)->mRawByte, 0x44);

	elementIt++;
	ASSERT_EQ((*elementIt)->mType, ht::Text::BlockElement::Type::RawByte);
	ASSERT_EQ((*elementIt)->mRawByte, 0x4D);

	elementIt++;
	ASSERT_EQ((*elementIt)->mType, ht::Text::BlockElement::Type::Text);
	ASSERT_TRUE((*elementIt)->mTextContent == U"BB[TestBlock2]CC");
}
