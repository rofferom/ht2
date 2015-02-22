#include <gtest/gtest.h>
#include <ht-legacy.hpp>

#define TABLE_FILE "test-res/ht-legacy/test-legacy-table.txt"

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
	const ht::Table::Entry *entry;
	int res;

	res = htl::loadTable(TABLE_FILE, "UTF-8", &table);
	ASSERT_EQ(res, 0);

	entry = table.findFromKey(key1.mValue, key1.mSize);
	ASSERT_TRUE(entry != NULL);
	ASSERT_TRUE(entry->mValue == U"a");

	entry = table.findFromKey(key2.mValue, key2.mSize);
	ASSERT_TRUE(entry != NULL);
	ASSERT_TRUE(entry->mValue == U"\n");

	entry = table.findFromKey(key3.mValue, key3.mSize);
	ASSERT_TRUE(entry != NULL);
	ASSERT_TRUE(entry->mValue == U"\nb");

	entry = table.findFromKey(key4.mValue, key4.mSize);
	ASSERT_TRUE(entry != NULL);
	ASSERT_TRUE(entry->mValue == U"\nb\n");

	entry = table.findFromKey(key5.mValue, key5.mSize);
	ASSERT_TRUE(entry != NULL);
	ASSERT_TRUE(entry->mValue == U"aa\\bbb");

	entry = table.findFromKey(key6.mValue, key6.mSize);
	ASSERT_TRUE(entry != NULL);
	ASSERT_TRUE(entry->mValue == U"aaa\\");
}
