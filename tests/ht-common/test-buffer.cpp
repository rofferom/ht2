#include <gtest/gtest.h>
#include <ht.hpp>

static void compareBuffer(ht::Buffer *buffer, const uint8_t *rawBuffer, size_t rawBufferSize)
{
	int res;

	res = buffer->getSize();
	EXPECT_EQ(res, rawBufferSize);

	EXPECT_TRUE(buffer->getData() != NULL);
	EXPECT_TRUE(memcmp(rawBuffer, buffer->getData(), rawBufferSize) == 0);

	for (size_t i = 0 ; i < rawBufferSize ; i++) {
		EXPECT_EQ(rawBuffer[i], buffer->getByte(i));
	}
}

TEST(Buffer, Test)
{
	ht::Buffer *buffer;
	int res;

	buffer = new ht::Buffer();
	EXPECT_TRUE(buffer != NULL);

	res = buffer->getCapacity();
	EXPECT_EQ(res, 0);

	res = buffer->getSize();
	EXPECT_EQ(res, 0);

	EXPECT_TRUE(buffer->getData() == NULL);;

	res = buffer->setSize(1);
	EXPECT_EQ(res, -EINVAL);

	// Set a buffer
	static uint8_t rawBytes1[] = { 1, 2, 3, 4, 5 };
	res = buffer->append(rawBytes1, HT_SIZEOF_ARRAY(rawBytes1));
	EXPECT_EQ(res, HT_SIZEOF_ARRAY(rawBytes1));

	compareBuffer(buffer, rawBytes1, HT_SIZEOF_ARRAY(rawBytes1));

	// Modify the content
	static uint8_t rawBytes2[] = { 10, 2, 3, 4, 15 };
	buffer->setByte(0, 10);
	buffer->setByte(4, 15);

	compareBuffer(buffer, rawBytes2, HT_SIZEOF_ARRAY(rawBytes2));

	// Test seek
	static uint8_t rawBytes3[] = { 3, 4, 15 };

	res = buffer->seek(200);
	EXPECT_EQ(res, -EINVAL);

	res = buffer->seek(2);
	EXPECT_EQ(res, 0);
	compareBuffer(buffer, rawBytes3, HT_SIZEOF_ARRAY(rawBytes3));

	static uint8_t rawBytes4[] = { 3, 24, 25 };
	static uint8_t rawBytes5[] = { 10, 2, 3, 24, 25 };

	buffer->setByte(1, 24);
	buffer->setByte(2, 25);

	compareBuffer(buffer, rawBytes4, HT_SIZEOF_ARRAY(rawBytes4));

	res = buffer->seek(0);
	EXPECT_EQ(res, 0);

	compareBuffer(buffer, rawBytes5, HT_SIZEOF_ARRAY(rawBytes5));

	// Test clear
	buffer->clear();

	res = buffer->getCapacity();
	EXPECT_EQ(res, 0);

	res = buffer->getSize();
	EXPECT_EQ(res, 0);

	EXPECT_TRUE(buffer->getData() == NULL);;

	// Test append + seek(2) + append + seek(0)
	res = buffer->append(rawBytes1, HT_SIZEOF_ARRAY(rawBytes1));
	EXPECT_EQ(res, HT_SIZEOF_ARRAY(rawBytes1));

	res = buffer->seek(2);
	EXPECT_EQ(res, 0);

	static uint8_t rawBytes6[] = { 100, 101, 102 };
	res = buffer->append(rawBytes6, HT_SIZEOF_ARRAY(rawBytes6));
	EXPECT_EQ(res, HT_SIZEOF_ARRAY(rawBytes6));

	res = buffer->getSize();
	EXPECT_EQ(res, 6);

	static uint8_t rawBytes7[] = { 3, 4, 5, 100, 101, 102 };
	compareBuffer(buffer, rawBytes7, HT_SIZEOF_ARRAY(rawBytes7));

	res = buffer->seek(0);
	EXPECT_EQ(res, 0);

	static uint8_t rawBytes8[] = { 1, 2, 3, 4, 5, 100, 101, 102 };
	compareBuffer(buffer, rawBytes8, HT_SIZEOF_ARRAY(rawBytes8));

	delete buffer;
}
