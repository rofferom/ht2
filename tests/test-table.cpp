#include <gtest/gtest.h>
#include <ht/Table.hpp>

struct TestKey {
	uint8_t mValue[10];
	size_t mSize;
};

static const TestKey key1 = {{0x11}, 1};
static const TestKey key2 = {{0x12}, 1};
static const TestKey key3 = {{0x12, 0x13}, 2};
static const TestKey key4 = {{0x12, 0x14}, 2};

TEST(Table, Test)
{
	ht::Table t;
	const ht::Table::Key *key;
	const std::u32string *value;
	int res;

	// Test insertion
	res = t.addEntry(key1.mValue, key1.mSize, U"A");
	ASSERT_EQ(res, 0);

	res = t.addEntry(key1.mValue, key1.mSize, U"A");
	ASSERT_NE(res, 0);

	res = t.addEntry(key2.mValue, key2.mSize, U"BBB");
	ASSERT_EQ(res, 0);

	res = t.addEntry(key3.mValue, key3.mSize, U"BBB");
	ASSERT_NE(res, 0);

	res = t.addEntry(key3.mValue, key3.mSize, U"CCCCC");
	ASSERT_EQ(res, 0);

	res = t.addEntry(key3.mValue, key3.mSize, U"C");
	ASSERT_NE(res, 0);

	// Test get
	key = t.findFromValue(U"A");
	ASSERT_TRUE(key != NULL);

	key = t.findFromValue(U"Z");
	ASSERT_TRUE(key == NULL);

	key = t.findFromValue(U"CCCCC");
	ASSERT_TRUE(key != NULL);

	value = t.findFromKey(key1.mValue, key1.mSize);
	ASSERT_TRUE(value != NULL);
	ASSERT_TRUE(*value == U"A");

	value = t.findFromKey(key3.mValue, key3.mSize);
	ASSERT_TRUE(value != NULL);
	ASSERT_TRUE(*value == U"CCCCC");

	value = t.findFromKey(key4.mValue, key4.mSize);
	ASSERT_TRUE(value == NULL);
}
