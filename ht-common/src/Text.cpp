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

int extractMatchingPointers(
	uint32_t offset,
	size_t start,
	const std::vector<ht::Text::Pointer *> &src,
	std::list<ht::Text::Pointer *> *dest)
{
	size_t srcSize = src.size();
	int res = 0;

	for (size_t i = start ; i < srcSize ; i++) {
		if (src[i]->mOffset == offset) {
			dest->push_back(src[i]);
			res++;
		} else {
			break;
		}
	}

	return res;
}

int decodeBuffer(
	const uint8_t *rawText,
	size_t rawTextSize,
	const ht::Table &table,
	ht::Text::Block *block)
{
	size_t maxKeySize = table.getMaxKeySize();
	size_t searchSize;
	ht::Text::BlockElement *currentElement = NULL;
	const ht::Table::Entry *entry;
	int res;

	res = 0;
	searchSize = std::min(maxKeySize, rawTextSize);

	while (rawTextSize > 0) {
		entry = table.findFromKey(rawText, searchSize);

		if ((entry == NULL) && (searchSize == 1)) {
			ht::Text::BlockElement *rawByteElement;

			rawByteElement = new ht::Text::BlockElement {
				ht::Text::BlockElement::Type::RawByte, U"", rawText[0] };
			if (rawByteElement == NULL) {
				res = -ENOMEM;
				break;
			}

			ht::Log::d(TAG, "Unknown byte %02X", rawByteElement->mRawByte);
			currentElement = NULL;
			block->mElementList.push_back(rawByteElement);

			rawText++;
			rawTextSize--;

			searchSize = std::min(maxKeySize, rawTextSize);
		} else if (entry == NULL) {
			searchSize--;
		} else {
			if (currentElement == NULL) {
				currentElement = new ht::Text::BlockElement {
					ht::Text::BlockElement::Type::Text, U"", 0 };
				if (currentElement == NULL) {
					res = -ENOMEM;
					break;
				}

				ht::Log::d(TAG, "New BlockElement of type Text");
				block->mElementList.push_back(currentElement);
			}

			currentElement->mTextContent.append(entry->mValue);

			rawText += entry->mKey.mSize;
			rawTextSize -= entry->mKey.mSize;

			searchSize = std::min(maxKeySize, rawTextSize);
		}
	}

	return res;
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
	const std::vector<Pointer *> &inPointerList)
{
	std::vector<Pointer *> pointerList;
	size_t pointerCount;
	size_t nextPointer;
	uint32_t textToExtractStart;
	size_t textToExtractSize;
	int foundPointerCount;
	int res;

	pointerList = inPointerList;
	std::sort(
		pointerList.begin(), pointerList.end(),
		[](const Pointer *x, const Pointer *y) {
			return (x->mOffset < y->mOffset); }
	);

	res = 0;
	pointerCount = pointerList.size();
	for (size_t i = 0 ; i < pointerCount ;) {
		Block *block = new Block();
		if (block == NULL) {
			res = -ENOMEM;
			break;
		}

		textToExtractStart = pointerList[i]->mOffset;

		// Find matching pointers
		foundPointerCount = extractMatchingPointers(
			textToExtractStart,
			i,
			pointerList,
			&block->mPointerList);

		// Find size of rawText to extract for this block
		nextPointer = i + foundPointerCount;
		if (nextPointer == pointerCount) {
			// End of text
			textToExtractSize = rawTextSize - textToExtractStart;
		} else {
			// Text followed by another block
			textToExtractSize = pointerList[nextPointer]->mOffset - textToExtractStart;
		}

		ht::Log::d(TAG,
			"Found %d pointers for a block of size %d start at offset %d",
			foundPointerCount,
			textToExtractSize,
			textToExtractStart);

		// Decode buffer
		res = decodeBuffer(rawText + textToExtractStart, textToExtractSize, table, block);
		if (res < 0) {
			break;
		}

		mBlockList.push_back(block);
		i = nextPointer;
	}

	return res;
}

} // namespace ht
