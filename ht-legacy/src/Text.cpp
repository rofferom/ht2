#include <stdio.h>
#include <vfprintf32.h>
#include <ht/Log.hpp>
#include <ht/CharsetConverter.hpp>
#include <ht-legacy/Text.hpp>

static const char32_t *TAG = U"htl.Text";

namespace {

static int writeToFile(const void *buff, size_t size, void *userdata)
{
	FILE *out = (FILE *) userdata;

	return fwrite(buff, 1, size, out);
}

static size_t printfCb(const char32_t *s, size_t size, void *userdata)
{
	struct ht::CharsetConverter *charsetConv;
	charsetConv = (struct ht::CharsetConverter *) userdata;

	return ht::charsetConverterInput(charsetConv, s, size * sizeof(char32_t));
}

} // anonymous namespace

namespace htl {

int loadText(const char *textPath, const char *encoding, ht::Text *text)
{
	return 0;
}

int saveText(const ht::Text *text, const char *textPath, const char *encoding)
{
	struct ht::CharsetConverter *charsetConv = NULL;
	struct ht::CharsetConverterCb convCb;
	struct OutputStream printfOut;
	FILE *out = NULL;
	size_t blockCount;
	int res;

	if (!text || !textPath || !encoding) {
		return -EINVAL;
	}

	out = fopen(textPath, "wb");
	if (out == NULL) {
		res = -EINVAL;
		goto out;
	}

	charsetConv = ht::charsetConverterCreate();
	if (charsetConv == NULL) {
		res = -ENOMEM;
		goto out;
	}

	convCb.invalid_sequence = NULL;
	convCb.output = writeToFile;
	convCb.userdata = out;

	res = ht::charsetConverterOpen(charsetConv, encoding, "UTF-32LE", &convCb);
	if (res < 0) {
		goto out;
	}

	printfOut.write = printfCb;
	printfOut.userdata = charsetConv;

	blockCount = text->getBlockCount();
	for (size_t i = 0 ; i < blockCount ; i++) {
		const ht::Text::Block *block = text->getBlock(i);

		for (const auto &ptr : block->mPointerList) {
			fprintf32(&printfOut, U"<PTR%04d>\n", ptr->mId);
		}

		for (const auto &elem : block->mElementList) {
			switch (elem->mType) {
			case ht::Text::BlockElement::Type::RawByte:
				fprintf32(&printfOut, U"<%02X>", elem->mRawByte);
				break;

			case ht::Text::BlockElement::Type::Text:
				fprintf32(&printfOut, U"%s", elem->mTextContent.c_str());
				break;

			default:
				break;
			}
		}
	}

	ht::charsetConverterClose(charsetConv);
	ht::charsetConverterDestroy(charsetConv);

	fclose(out);

	return 0;

out:
	if (out != NULL) {
		fclose(out);
	}

	ht::charsetConverterDestroy(charsetConv);
	return res;
}

} // namespace htl
