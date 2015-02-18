#include "ht-legacy-private.hpp"

#define POINTER_LEN 9
#define RAWBYTE_LEN 4

namespace {

enum class BlockType {
	None,
	RawByte,
	Pointer,
};

bool isDigit(char32_t c)
{
	return (U'0' <= c && c <= U'9');
}

int convertDigit(char32_t c)
{
	return c - U'0';
}

int getPointerId(const char32_t *s)
{
	int res;

	res = convertDigit(s[6]);
	res += convertDigit(s[5]) * 10;
	res += convertDigit(s[4]) * 100;
	res += convertDigit(s[3]) * 1000;

	return res;
}

bool isPointer(const char32_t *s, size_t size)
{
	// <PTxxxx>\n
	if (size < POINTER_LEN) {
		return false;
	}

	// Check global pattern
	if (s[0] != U'<' || s[1] != U'P' || s[2] != U'T' || s[7] != U'>' || s[8] != U'\n') {
		return false;
	}

	// Check pointer ID
	for (int i = 0 ; i < 4 ; i++) {
		if (isDigit(s[3+i]) == false) {
			return false;
		}
	}

	return true;
}

bool isHex(char32_t c)
{
	return (isDigit(c) || (U'a' <= c && c <= U'f') || (U'A' <= c && c <= U'F'));
}

uint8_t convertHexChar(char32_t c)
{
	uint8_t res;

	if (isDigit(c)) {
		res = c - U'0';
	} else if (U'a' <= c && c <= U'f') {
		res = (c - U'a') + 0xA;
	} else if (U'A' <= c && c <= U'F') {
		res = (c - U'A') + 0xA;
	} else {
		res = 0;
		assert(false);
	}

	return res;
}

uint8_t getRawByteContent(const char32_t *s)
{
	uint8_t res;

	res = convertHexChar(s[2]);
	res |= convertHexChar(s[1]) << 4;

	return res;
}

bool isRawByte(const char32_t *s, size_t size)
{
	// <xx>
	if (size < RAWBYTE_LEN) {
		return false;
	}

	return (s[0] == U'<' && isHex(s[1]) && isHex(s[2]) && s[3] == U'>');
}

const char32_t *findNextBlock(const char32_t *s, size_t size, BlockType *outType)
{
	for (size_t i = 0 ; i < size ; i++) {
		if (s[i] == U'<') {
			if (isPointer(s + i, size - i)) {
				*outType = BlockType::Pointer;
				return s + i;
			} else if (isRawByte(s + i, size - i)) {
				*outType = BlockType::RawByte;
				return s + i;
			}
		}
	}

	*outType = BlockType::None;

	return NULL;
}

} // anonymous namespace

namespace htl {

int tokenizeText(const char *textPath, const char *encoding, const TokenizerCb &cb)
{
	char32_t *content;
	size_t contentSize;
	int res;

	if (!textPath || !encoding) {
		return -EINVAL;
	}

	res = ht::TextFile::load(textPath, encoding, &content, &contentSize);
	if (res < 0) {
		goto error;
	}

	for (size_t i = 0 ; i < contentSize ;) {
		const char32_t *nextBlock;
		BlockType blockType;

		nextBlock = findNextBlock(content + i, contentSize - i, &blockType);
		if (nextBlock == NULL) {
			cb.textFound(content + i, contentSize - i, cb.userdata);
			break;
		} else {
			size_t textSize = nextBlock - (content + i);

			if (textSize > 0) {
				cb.textFound(content + i, textSize, cb.userdata);
			}

			if (blockType == BlockType::Pointer) {
				cb.pointerFound(getPointerId(nextBlock), cb.userdata);
				i = (nextBlock - content) + POINTER_LEN;
			} else if (blockType == BlockType::RawByte) {
				cb.rawByteFound(getRawByteContent(nextBlock), cb.userdata);
				i = (nextBlock - content) + RAWBYTE_LEN;
			}
		}
	}

	ht::TextFile::clear(content);

	return 0;

error:
	return res;
}

} // namespace htl
