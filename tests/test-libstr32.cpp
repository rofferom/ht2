#include <stdio.h>
#include <vfprintf32.h>
#include <gtest/gtest.h>

static size_t string_write(const char32_t *s, size_t size, void *userdata)
{
	std::u32string *out = (std::u32string *) userdata;

	out->append(s, size);

	return size;
}

TEST(libstr32, fprintf32)
{
	struct OutputStream out;
	std::u32string strOut;

	out.write = string_write;
	out.userdata = &strOut;

	fprintf32(&out, U"Hello world !");
	ASSERT_TRUE(strOut == U"Hello world !");
	strOut.clear();

	fprintf32(&out, U"Hello world %s !", U"Bob");
	ASSERT_TRUE(strOut == U"Hello world Bob !");
	strOut.clear();

	fprintf32(&out, U"Hello world 0x%03x (ファイナルファンタジー) %s !", 0x6a, U"Bob");
	ASSERT_TRUE(strOut == U"Hello world 0x06a (ファイナルファンタジー) Bob !");
	strOut.clear();

	fprintf32(&out, U"Hello world 0x%02x (ファイナルファンタジー) %s !", 0xa, U"Bob");
	ASSERT_TRUE(strOut == U"Hello world 0x0a (ファイナルファンタジー) Bob !");
	strOut.clear();

	fprintf32(&out, U"Hello world 0x%02X (ファイナルファンタジー) %s !", 0xa, U"Bob");
	ASSERT_TRUE(strOut == U"Hello world 0x0A (ファイナルファンタジー) Bob !");
	strOut.clear();

	fprintf32(&out, U"Hello world 0x%02x (ファイナルファンタジー) %d !", 0x6a, 100);
	ASSERT_TRUE(strOut == U"Hello world 0x6a (ファイナルファンタジー) 100 !");
	strOut.clear();

	fprintf32(&out, U"Hello world %p !", (void *) 0x100802000);
	ASSERT_TRUE(strOut == U"Hello world 0x100802000 !");
	strOut.clear();
}
