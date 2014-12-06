#include <string.h>
#include <gtest/gtest.h>
#include <ht-legacy/TableLoader.hpp>

#define TABLE_FILE "test-legacy-table.txt"

struct TestKey {
	uint8_t mValue[10];
	size_t mSize;
};

static const TestKey key1 = {{0x11}, 1};
static const TestKey key2 = {{0x12}, 1};
static const TestKey key3 = {{0x13}, 1};
static const TestKey key4 = {{0x14}, 1};
static const TestKey key5 = {{0x15}, 1};
static const TestKey key6 = {{0x16}, 1};

TEST(LegacyTableLoader, Test)
{
	ht::Table table;
	const std::u32string *value;
	int res;

	res = htl::loadTable(TABLE_FILE, "UTF-8", &table);
	ASSERT_EQ(res, 0);

	value = table.findFromKey(key1.mValue, key1.mSize);
	ASSERT_TRUE(value != NULL);
	ASSERT_TRUE(*value == U"a");

	value = table.findFromKey(key2.mValue, key2.mSize);
	ASSERT_TRUE(value != NULL);
	ASSERT_TRUE(*value == U"\n");

	value = table.findFromKey(key3.mValue, key3.mSize);
	ASSERT_TRUE(value != NULL);
	ASSERT_TRUE(*value == U"\nb");

	value = table.findFromKey(key4.mValue, key4.mSize);
	ASSERT_TRUE(value != NULL);
	ASSERT_TRUE(*value == U"\nb\n");

	value = table.findFromKey(key5.mValue, key5.mSize);
	ASSERT_TRUE(value != NULL);
	ASSERT_TRUE(*value == U"aa\\bbb");

	value = table.findFromKey(key6.mValue, key6.mSize);
	ASSERT_TRUE(value != NULL);
	ASSERT_TRUE(*value == U"aaa\\");
}
