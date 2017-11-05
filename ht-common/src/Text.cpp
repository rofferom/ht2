#include <errno.h>
#include <algorithm>
#include "ht-private.hpp"

static const char32_t *TAG = U"Text";

namespace ht {

Text::Decoder::Decoder(const Table *table, std::vector<Block *> *blockList)
{
	mTable = table;
	mBlockList = blockList;
	mCurrentBlock = nullptr;
}

int Text::Decoder::newBlock(std::vector<Pointer> **pointerList)
{
	mCurrentBlock = new Block();
	if (mCurrentBlock == nullptr) {
		return -ENOMEM;
	}

	*pointerList = &mCurrentBlock->mPointerList;

	return 0;
}

int Text::Decoder::blockData(const uint8_t *rawText, size_t rawTextSize)
{
	int res;

	res = decodeBuffer(rawText, rawTextSize, *mTable, mCurrentBlock);
	if (res < 0) {
		return res;
	}

	mBlockList->push_back(mCurrentBlock);
	mCurrentBlock = nullptr;

	return 0;
}

Text::DecoderRaw::DecoderRaw(std::vector<RawBlock *> *blockList)
{
	mBlockList = blockList;
	mCurrentBlock = nullptr;
}

int Text::DecoderRaw::newBlock(std::vector<Pointer> **pointerList)
{
	mCurrentBlock = new RawBlock();
	if (mCurrentBlock == nullptr) {
		return -ENOMEM;
	}

	*pointerList = &mCurrentBlock->mPointerList;

	return 0;
}

int Text::DecoderRaw::blockData(const uint8_t *rawText, size_t rawTextSize)
{
	int res;

	res = mCurrentBlock->mBuffer.append(rawText, rawTextSize);
	if (res < 0) {
		return res;
	}

	mBlockList->push_back(mCurrentBlock);
	mCurrentBlock = nullptr;

	return 0;
}

Text::Text()
{
}

Text::~Text()
{
	for (Block *block : mBlockList) {
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
	PointerTable *pointerList) const
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
			pointer.mOffset = writePos;
			pointerList->add(pointer);
		}

		res = 0;
	}

	return res;
}

int Text::extractMatchingPointers(
	uint32_t offset,
	size_t start,
	const ht::PointerTable &src,
	std::vector<ht::Pointer> *dest)
{
	size_t srcSize = src.getCount();
	int res = 0;

	for (size_t i = start ; i < srcSize ; i++) {
		const ht::Pointer *pointer = src.getPointer(i);

		if (pointer->mOffset == offset) {
			dest->push_back(*pointer);
			res++;
		} else {
			break;
		}
	}

	return res;
}

int Text::decode(
	const Buffer &buffer,
	const Table &table,
	const PointerTable &inPointerTable)
{
	using std::placeholders::_1;
	using std::placeholders::_2;

	Decoder decoder(&table, &mBlockList);
	SplitCb cb;

	cb.mNewBlock = std::bind(&Decoder::newBlock, &decoder, _1);
	cb.mBlockData = std::bind(&Decoder::blockData, &decoder, _1, _2);

	return splitText(buffer, inPointerTable, cb);
}

int Text::encodeString(
	const std::u32string &s,
	const ht::Table &table,
	ht::Buffer *buffer,
	const TxtLocation *sLoc)
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
			if (sLoc) {
				TxtLocation charLoc;

				findLocFromString(s.c_str(), sLoc, sIt, &charLoc);

				ht::Log::e(TAG, U"Invalid char found: %c (%d:%d)",
						*sIt, charLoc.mLine, charLoc.mColumn);
			} else {
				ht::Log::e(TAG, U"Invalid char found: %c", *sIt);
			}
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

int Text::encodeBlock(
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
				buffer,
				&elem->mTextLoc);
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

int Text::splitText(
	const Buffer &buffer,
	const PointerTable &inPointerTable,
	SplitCb &cb)
{
	Pointer *pointer;
	PointerTable pointerTable;
	size_t pointerCount;
	size_t nextPointerId;
	uint32_t textToExtractStart;
	size_t textToExtractSize;
	int foundPointerCount;
	const uint8_t *rawText;
	size_t rawTextSize;
	int res;

	pointerCount = inPointerTable.getCount();
	if (pointerCount == 0) {
		ht::Log::e(TAG, U"Pointer table is empty");
		return -EINVAL;
	}

	rawText = buffer.getData();
	rawTextSize = buffer.getSize();

	pointerTable = inPointerTable;
	pointerTable.sort(
		[](const Pointer &x, const Pointer &y) {
			return (x.mOffset < y.mOffset); }
	);

	pointer = pointerTable.getPointer(0);
	if (pointer == nullptr) {
		ht::Log::e(TAG, U"Unable to fetch first pointer");
		return -EINVAL;
	} else if (pointer->mOffset > rawTextSize) {
		ht::Log::e(TAG,
			U"First pointer offset is out of range "
			U"(Pointer offset : %X - Text size : %X)",
			pointer->mOffset, rawTextSize);
		return -EINVAL;
	}

	res = 0;
	for (size_t i = 0 ; i < pointerCount ;) {
		std::vector<Pointer> *blockPointers;

		res = cb.mNewBlock(&blockPointers);
		if (res < 0) {
			break;
		}

		pointer = pointerTable.getPointer(i);
		textToExtractStart = pointer->mOffset;

		// Find matching pointers
		foundPointerCount = extractMatchingPointers(
			textToExtractStart,
			i,
			pointerTable,
			blockPointers);

		// Find size of rawText to extract for this block
		nextPointerId = i + foundPointerCount;
		if (nextPointerId == pointerCount) {
			// End of text
			textToExtractSize = rawTextSize - textToExtractStart;
		} else {
			Pointer *nextPointer;
			// Text followed by another block
			nextPointer = pointerTable.getPointer(nextPointerId);
			textToExtractSize = nextPointer->mOffset - textToExtractStart;
		}

		ht::Log::d(TAG,
			U"Found %d pointers for a block of size %d start at offset %d",
			foundPointerCount,
			textToExtractSize,
			textToExtractStart);

		// Decode buffer
		res = cb.mBlockData(rawText + textToExtractStart, textToExtractSize);
		if (res < 0) {
			break;
		}

		i = nextPointerId;
	}

	return res;
}

int Text::findLocFromString(const char32_t *s, const TxtLocation *baseLoc,
			const char32_t *cPtr, TxtLocation *charLoc)
{
	size_t targetIdx = cPtr - s;

	*charLoc = *baseLoc;
	for (size_t i = 0; i < targetIdx; i++) {
		if (s[i] == U'\n') {
			charLoc->mColumn = 1;
			charLoc->mLine++;
		} else {
			charLoc->mColumn++;
		}
	}

	return 0;
}

int Text::decodeBuffer(
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

int Text::splitText(
	const Buffer &buffer,
	const PointerTable &pointerList,
	std::vector<RawBlock *> *splittedBuffer)
{
	using std::placeholders::_1;
	using std::placeholders::_2;

	DecoderRaw decoder(splittedBuffer);
	SplitCb cb;

	if (splittedBuffer == NULL) {
		return -EINVAL;
	}

	cb.mNewBlock = std::bind(&DecoderRaw::newBlock, &decoder, _1);
	cb.mBlockData = std::bind(&DecoderRaw::blockData, &decoder, _1, _2);

	return splitText(buffer, pointerList, cb);
}

int Text::splitRawText(
		const Buffer &buffer,
		uint8_t separator,
		std::vector<Buffer *> *bufferList)
{
	Buffer *newBuffer;
	const uint8_t *blockStart;
	const uint8_t *data;
	size_t size;
	size_t i;

	if (bufferList == NULL) {
		return -EINVAL;
	}

	data = buffer.getData();
	size = buffer.getSize();

	blockStart = data;
	for (i = 0; i < size; i++) {
		if (data[i] == separator) {
			newBuffer = new Buffer(blockStart, i - (blockStart - data) + 1);
			if (newBuffer == NULL) {
				return -ENOMEM;
			}

			bufferList->push_back(newBuffer);
			blockStart = data + i + 1;
		}
	}

	if ((size_t) (blockStart - data) <= size) {
		newBuffer = new Buffer(blockStart, size - (blockStart - data));
		if (newBuffer == NULL) {
			return -ENOMEM;
		}
	}

	bufferList->push_back(newBuffer);

	return 0;
}

} // namespace ht
