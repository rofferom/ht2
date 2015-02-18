#include <gtest/gtest.h>
#include <ht-legacy.hpp>

static const char32_t *TAG = U"test.legacytokenizer";

enum class BlockType {
	RawByte,
	Pointer,
	Text,
};

struct BlockDescr {
	BlockType mType;
	int mValue;
};

static const BlockDescr blocks[] = {
	{BlockType::Pointer, 1},
	{BlockType::Text, 5},
	{BlockType::RawByte, 0x43},
	{BlockType::Text, 14},
	{BlockType::RawByte, 0x44},
	{BlockType::Pointer, 2},
	{BlockType::Pointer, 3},
	{BlockType::Pointer, 4},
	{BlockType::Text, 16},
	{BlockType::RawByte, 0x43},
	{BlockType::RawByte, 0x44},
	{BlockType::RawByte, 0x4D},
	{BlockType::Text, 16},
};

size_t blocksCount = HT_SIZEOF_ARRAY(blocks);

struct Context {
	int mPointerCount;
	int mRawByteCount;
	int mTextFoundCount;
	int mCurrentBlock;

	Context()
	{
		mPointerCount = 0;
		mRawByteCount = 0;
		mTextFoundCount = 0;
		mCurrentBlock = 0;
	}
};

static void pointerFound(uint32_t id, void *userdata)
{
	Context *context = (Context *) userdata;

	ht::Log::i(TAG, U"Pointer %d found", id);

	ASSERT_EQ(blocks[context->mCurrentBlock].mType, BlockType::Pointer);
	ASSERT_EQ(blocks[context->mCurrentBlock].mValue, id);

	context->mPointerCount++;
	context->mCurrentBlock++;
}

static void rawByteFound(uint8_t byte, void *userdata)
{
	Context *context = (Context *) userdata;

	ht::Log::i(TAG, U"RawByte %02X found", byte);

	ASSERT_EQ(blocks[context->mCurrentBlock].mType, BlockType::RawByte);
	ASSERT_EQ(blocks[context->mCurrentBlock].mValue, byte);

	context->mRawByteCount++;
	context->mCurrentBlock++;
}

static void textFound(const char32_t *s, size_t size, void *userdata)
{
	Context *context = (Context *) userdata;

	ht::Log::i(TAG, U"Text found (len %d)", size);

	ASSERT_EQ(blocks[context->mCurrentBlock].mType, BlockType::Text);
	ASSERT_EQ(blocks[context->mCurrentBlock].mValue, size);

	context->mTextFoundCount++;
	context->mCurrentBlock++;
}

TEST(TokenizerLegacy, Base)
{
	struct htl::TokenizerCb cb;
	Context context;
	int res;

	cb.pointerFound = pointerFound;
	cb.rawByteFound = rawByteFound;
	cb.textFound = textFound;
	cb.userdata = &context;

	res = htl::tokenizeText("test-legacytext-save.txt", "UTF-8", cb);
	ASSERT_EQ(res, 0);
	ASSERT_EQ(context.mPointerCount, 4);
	ASSERT_EQ(context.mRawByteCount, 5);
	ASSERT_EQ(context.mTextFoundCount, 4);
}
