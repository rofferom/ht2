#include <string.h>
#include <gtest/gtest.h>
#include <ht/Log.hpp>
#include <ht/TextFile.hpp>
#include <ht/Utils.hpp>

#define UTF8_FILE "test-textfile-utf8.txt"
#define UTF32_FILE "test-textfile-utf32.txt"

static const char *TAG = "TEST.textfile";

TEST(TextFile, CharsetConversion)
{
	char32_t *content = NULL;
	size_t contentSize;
	char *expectedContent = NULL;
	size_t expectedContentSize;
	int res;

	res = ht::TextFile::load(UTF8_FILE, "UTF-8", &content, &contentSize);
	if (res < 0) {
		ht::Log::e(TAG, "Error during file %s loading", UTF8_FILE);
		ASSERT_TRUE(false);
	}

	res = ht::loadFile(UTF32_FILE, &expectedContent, &expectedContentSize);
	if (res < 0) {
		ht::Log::e(TAG, "Error during file %s loading", UTF32_FILE);
		ASSERT_TRUE(false);
	}

	ht::Log::i(TAG, "No conversion error, compare to expected result");

	if (contentSize * sizeof(char32_t) != expectedContentSize) {
		ht::Log::e(TAG, "Converted size is %d, expected %d",
			contentSize * sizeof(char32_t),
			expectedContentSize);
		ASSERT_TRUE(false);
	}

	if (memcmp(content, expectedContent, expectedContentSize) != 0) {
		ht::Log::e(TAG, "Converted data invalid");
		ASSERT_TRUE(false);
	}

	ht::TextFile::clear(content);
	free(expectedContent);
}
