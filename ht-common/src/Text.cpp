#include <errno.h>
#include <algorithm>
#include <ht/Log.hpp>
#include <ht/Text.hpp>

static const char *TAG = "Text";

namespace {

uint8_t *checkSize(uint8_t *array, size_t *size, size_t requiredSize)
{
	if (*size > requiredSize) {
		return array;
	} else {
		if (*size == 0) {
			*size = 1024;
		} else {
			*size *= 2;
		}

		return (uint8_t *) realloc(array, *size);
	}
}

int encodeString(
	const std::u32string &s,
	const ht::Table &table,
	uint8_t **outRawText,
	size_t *outRawTextSize,
	size_t writePos)
{
	size_t maxValueSize = table.getMaxValueSize();
	uint8_t *rawText;
	size_t rawTextSize;
	const char32_t *sIt;
	size_t sItSize;
	size_t searchSize;
	const ht::Table::Entry *entry;
	int res;

	rawText = *outRawText;
	rawTextSize = *outRawTextSize;

	res = 0;
	sIt = s.c_str();
	sItSize = s.size();
	searchSize = std::min(maxValueSize, sItSize);

	while (*sIt != U'\0') {
		entry = table.findFromValue(std::u32string(sIt, searchSize));
		if ((entry == NULL) && (searchSize == 1)) {
			ht::Log::e(TAG, "Invalid char found");
			res = -EINVAL;
			break;
		} else if (entry == NULL) {
			searchSize--;
		} else {
			rawText = checkSize(rawText, &rawTextSize, writePos + entry->mKey.mSize);

			ht::Log::d(TAG, "Writing %d bytes at %p", entry->mKey.mSize, rawText + writePos);
			memcpy(rawText + writePos, entry->mKey.mValue, entry->mKey.mSize);
			writePos += entry->mKey.mSize;
			res += entry->mKey.mSize;

			sIt += searchSize;
			sItSize -= searchSize;

			searchSize = std::min(maxValueSize, sItSize);
		}
	}

	if (res > 0) {
		*outRawText = rawText;
		*outRawTextSize = rawTextSize;
	}

	return res;
}

int encodeBlock(
	const ht::Text::Block *block,
	const ht::Table &table,
	uint8_t **outRawText,
	size_t *outRawTextSize,
	size_t inWritePos)
{
	size_t writePos = inWritePos;
	uint8_t *rawText;
	size_t rawTextSize;
	int res = 0;

	ht::Log::d(TAG, "Encoding new Block");

	rawText = *outRawText;
	rawTextSize = *outRawTextSize;

	for (auto elem : block->mElementList) {
		switch (elem->mType) {
		case ht::Text::BlockElement::Type::Text:
			ht::Log::d(TAG, "Encoding new BlockElement of type Text");

			res = encodeString(
				elem->mTextContent,
				table,
				&rawText,
				&rawTextSize,
				writePos);
			if (res > 0) {
				ht::Log::d(TAG, "%d bytes encoded", res);
				writePos += res;
			}

			break;

		case ht::Text::BlockElement::Type::RawByte:
			ht::Log::d(TAG, "Encoding new BlockElement of type RawByte");

			rawText = checkSize(rawText, &rawTextSize, writePos + 1);
			ht::Log::d(TAG, "Writing 1 byte at %p", rawText + writePos);
			rawText[writePos] = elem->mRawByte;
			writePos++;
			break;

		default:
			ht::Log::e(TAG, "Invalid BlockElement type %s", elem->mType);
			res = -EINVAL;
			break;
		}

		if (res < 0) {
			break;
		}
	}

	if (res < 0) {
		return res;
	} else {
		*outRawText = rawText;
		*outRawTextSize = rawTextSize;
		return (writePos - inWritePos);
	}
}

}

namespace ht {

Text::Text()
{
}

Text::~Text()
{
	for (Block *block : mBlockList) {
		for (Pointer *pointer : block->mPointerList) {
			delete pointer;
		}

		for (BlockElement *element : block->mElementList) {
			delete element;
		}

		delete block;
	}
}

int Text::addBlock(Block *block)
{
	mBlockList.push_back(block);
	return 0;
}

size_t Text::getBlockCount() const
{
	return mBlockList.size();
}

Text::Block *Text::getBlock(size_t index)
{
	if (index >= mBlockList.size()) {
		return NULL;
	}

	return mBlockList[index];
}

int Text::encode(
	const Table &table,
	uint8_t **outRawText,
	size_t *outRawTextSize,
	std::list<Pointer *> *pointerList) const
{
	int res;
	size_t writePos;

	if (outRawText == NULL || outRawTextSize == NULL || pointerList == NULL) {
		return -EINVAL;
	}

	*outRawText = NULL;
	*outRawTextSize = 0;

	res = 0;
	writePos = 0;

	for (auto block : mBlockList) {
		res = encodeBlock(
			block,
			table,
			outRawText,
			outRawTextSize,
			writePos);
		if (res < 0) {
			break;
		}

		for (auto pointer : block->mPointerList) {
			pointer->mOffset = writePos;
			pointerList->push_back(pointer);
		}

		writePos += res;
		res = 0;
	}

	if (res == 0) {
		*outRawTextSize = writePos;
	}

	return res;
}

int Text::decode(
	const uint8_t *rawText,
	size_t rawTextSize,
	const Table &table,
	const std::list<Pointer *> &pointerList)
{
	return 0;
}

} // namespace ht
