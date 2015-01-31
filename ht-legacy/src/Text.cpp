#include <stdio.h>
#include <vfprintf32.h>
#include <ht/Log.hpp>
#include <ht/CharsetConverter.hpp>
#include <ht-legacy/Text.hpp>
#include <ht-legacy/TextTokenizer.hpp>

static const char32_t *TAG = U"htl.Text";

namespace {

enum class LoaderState {
	Idle,
	Pointer,
	Text
};

struct Loader {
	LoaderState state;
	ht::Text *text;
	ht::Text::Block *currentBlock;

	Loader()
	{
		state = LoaderState::Idle;
		text = NULL;
		currentBlock = NULL;
	}
};

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

void pointerFound(uint32_t id, void *userdata)
{
	Loader *loader = (Loader *) userdata;

	switch (loader->state) {
	case LoaderState::Idle:
	case LoaderState::Text:
		loader->state = LoaderState::Pointer;

		loader->currentBlock = new ht::Text::Block();
		loader->text->addBlock(loader->currentBlock);
		break;

	case LoaderState::Pointer:
	default:
		break;
	}

	loader->currentBlock->mPointerList.push_back(ht::Pointer{id, 0, 0});
}

void rawByteFound(uint8_t byte, void *userdata)
{
	Loader *loader = (Loader *) userdata;
	ht::Text::BlockElement *elem;

	if (loader->currentBlock == NULL) {
		loader->currentBlock = new ht::Text::Block();
		loader->text->addBlock(loader->currentBlock);
	}

	loader->state = LoaderState::Text;

	elem = new ht::Text::BlockElement();
	elem->mType = ht::Text::BlockElement::Type::RawByte;
	elem->mRawByte = byte;

	loader->currentBlock->mElementList.push_back(elem);
}

void textFound(const char32_t *s, size_t size, void *userdata)
{
	Loader *loader = (Loader *) userdata;
	ht::Text::BlockElement *elem;

	if (loader->currentBlock == NULL) {
		loader->currentBlock = new ht::Text::Block();
		loader->text->addBlock(loader->currentBlock);
	}

	loader->state = LoaderState::Text;

	elem = new ht::Text::BlockElement();
	elem->mType = ht::Text::BlockElement::Type::Text;
	elem->mTextContent.assign(s, size);

	loader->currentBlock->mElementList.push_back(elem);
}

} // anonymous namespace

namespace htl {

int loadText(const char *textPath, const char *encoding, ht::Text *text)
{
	struct TokenizerCb cb;
	Loader loader;

	loader.text = text;

	cb.pointerFound = pointerFound;
	cb.rawByteFound = rawByteFound;
	cb.textFound = textFound;
	cb.userdata = &loader;

	return tokenizeText(textPath, encoding, cb);
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
			fprintf32(&printfOut, U"<PT%04d>\n", ptr.mId);
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
