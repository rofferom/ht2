#include "ht-legacy-private.hpp"

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

static size_t printfCb(const char32_t *s, size_t size, void *userdata)
{
	ht::CharsetConverter *charsetConv;
	charsetConv = (ht::CharsetConverter *) userdata;

	return charsetConv->input(s, size * sizeof(char32_t));
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

void textFound(const char32_t *s, size_t size,
		const ht::Text::TxtLocation &loc, void *userdata)
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
	elem->mTextLoc = loc;
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
	ht::CharsetConverter *charsetConv = NULL;
	ht::CharsetConverter::Cb convCb;
	struct OutputStream printfOut;
	FILE *out = NULL;
	size_t blockCount;
	int res;

	if (!text || !textPath || !encoding) {
		return -EINVAL;
	}

	out = fopen(textPath, "wb");
	if (out == NULL) {
		ht::Log::w(
			TAG, U"Unable to open file '%s' : %s",
			ht::SysEnv::toInternal(textPath).c_str(),
			ht::SysEnv::toInternal(strerror(errno)).c_str());

		res = -EINVAL;
		goto out;
	}

	charsetConv = ht::CharsetConverter::create();
	if (charsetConv == NULL) {
		res = -ENOMEM;
		goto out;
	}

	convCb.output = [out] (const void *buff, size_t size) -> int {
		return fwrite(buff, 1, size, out);
	};

	res = charsetConv->open(encoding, ht::InternalCharset::name, convCb);
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

	charsetConv->close();
	ht::CharsetConverter::destroy(charsetConv);

	fclose(out);

	return 0;

out:
	if (out != NULL) {
		fclose(out);
	}

	ht::CharsetConverter::destroy(charsetConv);
	return res;
}

} // namespace htl
