#include <string>
#include <gtest/gtest.h>
#include <ht/CharsetConverter.hpp>

int output(const void *buff, size_t size, void *userdata)
{
	std::u32string *out = (std::u32string *) userdata;

	out->append((const char32_t *) buff, size / sizeof(char32_t));

	return 0;
}

TEST(CharsetConverter, Test)
{
	const char *input = "ABCé^à€";
	const char32_t *expectedOutput = U"ABCé^à€";
	size_t inputSize = strlen(input);
	struct ht::CharsetConverter *converter;
	struct ht::CharsetConverterCb cb;
	std::u32string out;
	int res;

	cb.output = output;
	cb.userdata = &out;

	converter = ht::charsetConverterCreate();
	ASSERT_TRUE(converter != NULL);

	res = ht::charsetConverterOpen(converter, "UTF-32LE", "UTF-8", &cb);
	ASSERT_EQ(res, 0);

	res = ht::charsetConverterInput(converter, input, inputSize);
	ASSERT_EQ(res, 0);
	ASSERT_EQ(out.size(), 7);
	ASSERT_TRUE(out == expectedOutput);

	res = ht::charsetConverterClose(converter);
	ASSERT_EQ(res, 0);

	ht::charsetConverterDestroy(converter);
	converter = NULL;
}
