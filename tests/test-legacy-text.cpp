#include <string.h>
#include <gtest/gtest.h>
#include <ht/Log.hpp>
#include <ht/Text.hpp>
#include <ht-legacy/Text.hpp>
#include <ht/Utils.hpp>

TEST(TextLegacy, Save)
{
	ht::Text text;
	ht::Text::Block *block;
	size_t rawTextSize;
	int res;

	// Fill text
	block = new ht::Text::Block();
	block->mPointerList.push_back(new ht::Text::Pointer{1, 0, 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"AAÉDC", 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x43});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::Text, U"ÉDCBA<PTR000A>\n", 0});
	block->mElementList.push_back(new ht::Text::BlockElement{ht::Text::BlockElement::Type::RawByte, U"", 0x44});
	text.addBlock(block);

	block = new ht::Text::Block();
	block->mPointerList.push_back(new ht::Text::Pointer{2, 0, 0});
	block->mPointerList.push_back(new ht::Text::Pointer{3, 0, 0});
	block->mPointerList.push_back(new ht::Text::Pointer{4, 0, 0});
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
