#include <string.h>
#include <gtest/gtest.h>
#include <ht/Utils.hpp>
#include <ht/TableParser.hpp>

#define TABLE_FILE "test-table.txt"
#define TABLE_FILE_EMPTYKEY "test-table-emptykey.txt"
#define TABLE_FILE_INVALIDKEYCHAR "test-table-invalidkeychar.txt"
#define TABLE_FILE_EMPTYVALUE "test-table-emptyvalue.txt"
#define TABLE_FILE_NOSEPARATOR "test-table-noseparator.txt"


struct Entry {
	uint8_t key[3];
	size_t keySize;
	std::u32string value;
};

static const Entry entries[] = {
	{{0x11}, 1, U"a"},
	{{0x12}, 1, U"b"},
	{{0x13}, 1, U"ç"},
	{{0x14}, 1, U"É"},
	{{0x15}, 1, U"abc"},
	{{0x17}, 1, U"EéÇê€"},
	{{0x50, 0x00}, 2, U"a"},
	{{0x06, 0x00, 0x00}, 3, U"a"}
};

struct TestParser {
	int mLineCount;

	TestParser()
	{
		mLineCount = 0;
	}

	void entryFound(const uint8_t *key, size_t keySize, const std::u32string &value)
	{
		ASSERT_EQ(keySize, entries[mLineCount].keySize);
		ASSERT_EQ(memcmp(key, entries[mLineCount].key, keySize), 0);
		ASSERT_TRUE(value == entries[mLineCount].value);

		mLineCount++;
	}
};

TEST(TableParserParse, ValidTable)
{
	TestParser testParser;
	ht::TableParserNewEntryCb::Type cb = ht::TableParserNewEntryCb::get(&testParser, &TestParser::entryFound);
	int res;

	res = ht::parseTable(TABLE_FILE, "utf-8", cb);
	ASSERT_EQ(res, 0);
	ASSERT_EQ(testParser.mLineCount, 8);
}

TEST(TableParserParse, InvalidKeyChar)
{
	TestParser testParser;
	ht::TableParserNewEntryCb::Type cb = ht::TableParserNewEntryCb::get(&testParser, &TestParser::entryFound);
	int res;

	res = ht::parseTable(TABLE_FILE_INVALIDKEYCHAR, "utf-8", cb);
	ASSERT_NE(res, 0);
}

TEST(TableParserParse, EmptyKey)
{
	TestParser testParser;
	ht::TableParserNewEntryCb::Type cb = ht::TableParserNewEntryCb::get(&testParser, &TestParser::entryFound);
	int res;

	res = ht::parseTable(TABLE_FILE_EMPTYKEY, "utf-8", cb);
	ASSERT_NE(res, 0);
}

TEST(TableParserParse, EmptyValue)
{
	TestParser testParser;
	ht::TableParserNewEntryCb::Type cb = ht::TableParserNewEntryCb::get(&testParser, &TestParser::entryFound);
	int res;

	res = ht::parseTable(TABLE_FILE_EMPTYVALUE, "utf-8", cb);
	ASSERT_NE(res, 0);
}

TEST(TableParserParse, NoSeparator)
{
	TestParser testParser;
	ht::TableParserNewEntryCb::Type cb = ht::TableParserNewEntryCb::get(&testParser, &TestParser::entryFound);
	int res;

	res = ht::parseTable(TABLE_FILE_NOSEPARATOR, "utf-8", cb);
	ASSERT_NE(res, 0);
}
