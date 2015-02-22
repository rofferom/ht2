#include <gtest/gtest.h>
#include <ht.hpp>

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
	const ht::Table::Entry *entry;
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
	entry = t.findFromValue(U"A");
	ASSERT_TRUE(entry != NULL);
	ASSERT_EQ(entry->mKey.mSize, key1.mSize);
	ASSERT_EQ(memcmp(entry->mKey.mValue, key1.mValue, key1.mSize), 0);

	entry = t.findFromValue(U"Z");
	ASSERT_TRUE(entry == NULL);

	entry = t.findFromValue(U"CCCCC");
	ASSERT_TRUE(entry != NULL);
	ASSERT_EQ(entry->mKey.mSize, key3.mSize);
	ASSERT_EQ(memcmp(entry->mKey.mValue, key3.mValue, key3.mSize), 0);

	entry = t.findFromKey(key1.mValue, key1.mSize);
	ASSERT_TRUE(entry != NULL);
	ASSERT_TRUE(entry->mValue == U"A");

	entry = t.findFromKey(key3.mValue, key3.mSize);
	ASSERT_TRUE(entry != NULL);
	ASSERT_TRUE(entry->mValue == U"CCCCC");

	entry = t.findFromKey(key4.mValue, key4.mSize);
	ASSERT_TRUE(entry == NULL);

	// Test max sizes
	ASSERT_EQ(t.getMaxKeySize(), 2);
	ASSERT_EQ(t.getMaxValueSize(), 5);
}
