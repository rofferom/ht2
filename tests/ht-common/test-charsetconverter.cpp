#include <gtest/gtest.h>
#include <ht.hpp>

TEST(CharsetConverter, Test)
{
	const char *input = "ABCé^à€";
	const char32_t *expectedOutput = U"ABCé^à€";
	size_t inputSize = strlen(input);
	struct ht::CharsetConverter *converter;
	struct ht::CharsetConverter::Cb cb;
	std::u32string out;
	int res;

	cb.output = [&out] (const void *buff, size_t size) -> int {
		out.append((const char32_t *) buff, size / sizeof(char32_t));
		return 0;
	};

	converter = ht::CharsetConverter::create();
	ASSERT_TRUE(converter != NULL);

	res = converter->open("UTF-32LE", "UTF-8", cb);
	ASSERT_EQ(res, 0);

	res = converter->input(input, inputSize);
	ASSERT_EQ(res, 0);
	ASSERT_EQ(out.size(), 7);
	ASSERT_TRUE(out == expectedOutput);

	res = converter->close();
	ASSERT_EQ(res, 0);

	ht::CharsetConverter::destroy(converter);
	converter = NULL;
}
