#include <errno.h>
#include <algorithm>
#include <ht/Log.hpp>
#include <ht/Text.hpp>

static const char32_t *TAG = U"Text";

namespace {

int encodeString(
	const std::u32string &s,
	const ht::Table &table,
	ht::Buffer *buffer)
{
	size_t maxValueSize = table.getMaxValueSize();
	const char32_t *sIt;
	size_t sItSize;
	size_t searchSize;
	const ht::Table::Entry *entry;
	int res;

	res = 0;
	sIt = s.c_str();
	sItSize = s.size();
	searchSize = std::min(maxValueSize, sItSize);

	while (*sIt != U'\0') {
		entry = table.findFromValue(std::u32string(sIt, searchSize));
		if ((entry == NULL) && (searchSize == 1)) {
			ht::Log::e(TAG, U"Invalid char found");
			res = -EINVAL;
			break;
		} else if (entry == NULL) {
			searchSize--;
		} else {
			ht::Log::d(TAG, U"Writing %d bytes", entry->mKey.mSize);
			buffer->append(entry->mKey.mValue, entry->mKey.mSize);
			res += entry->mKey.mSize;

			sIt += searchSize;
			sItSize -= searchSize;

			searchSize = std::min(maxValueSize, sItSize);
		}
	}

	return res;
}

int encodeBlock(
	const ht::Text::Block *block,
	const ht::Table &table,
	ht::Buffer *buffer)
{
	size_t inSize = buffer->getSize();
	int res = 0;

	ht::Log::d(TAG, U"Encoding new Block");

	for (auto elem : block->mElementList) {
		switch (elem->mType) {
		case ht::Text::BlockElement::Type::Text:
			ht::Log::d(TAG, U"Encoding new BlockElement of type Text");

			res = encodeString(
				elem->mTextContent,
				table,
				buffer);
			if (res > 0) {
				ht::Log::d(TAG, U"%d bytes encoded", res);
			}

			break;

		case ht::Text::BlockElement::Type::RawByte:
			ht::Log::d(TAG, U"Encoding new BlockElement of type RawByte");
			ht::Log::d(TAG, U"Writing 1 byte at %p");
			buffer->append(&elem->mRawByte, 1);
			break;

		default:
			ht::Log::e(TAG, U"Invalid BlockElement type %s", elem->mType);
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
		return (buffer->getSize() - inSize);
	}
}

int extractMatchingPointers(
	uint32_t offset,
	size_t start,
	const std::vector<ht::Text::Pointer *> &src,
	std::vector<ht::Text::Pointer *> *dest)
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

			ht::Log::d(TAG, U"Unknown byte %02X", rawByteElement->mRawByte);
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

				ht::Log::d(TAG, U"New BlockElement of type Text");
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

const Text::Block *Text::getBlock(size_t index) const
{
	if (index >= mBlockList.size()) {
		return NULL;
	}

	return mBlockList[index];
}

int Text::encode(
	const Table &table,
	Buffer *buffer,
	PointerList *pointerList) const
{
	int res;

	if (buffer == NULL || pointerList == NULL) {
		return -EINVAL;
	}

	res = 0;

	for (auto block : mBlockList) {
		size_t writePos = buffer->getSize();

		res = encodeBlock(
			block,
			table,
			buffer);
		if (res < 0) {
			break;
		}

		for (auto pointer : block->mPointerList) {
			pointer->mOffset = writePos;
			pointerList->push_back(pointer);
		}

		res = 0;
	}

	return res;
}

int Text::decode(
	const Buffer &buffer,
	const Table &table,
	const PointerList &inPointerList)
{
	PointerList pointerList;
	size_t pointerCount;
	size_t nextPointer;
	uint32_t textToExtractStart;
	size_t textToExtractSize;
	int foundPointerCount;
	const uint8_t *rawText;
	size_t rawTextSize;
	int res;

	rawText = buffer.getData();
	rawTextSize = buffer.getSize();

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
			U"Found %d pointers for a block of size %d start at offset %d",
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
