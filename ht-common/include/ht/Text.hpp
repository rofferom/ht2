#ifndef __HT_TEXT_HPP__
#define __HT_TEXT_HPP__

#include <string>
#include <list>
#include <vector>
#include <ht/Table.hpp>

namespace ht {

class Text {
public:
	struct Pointer {
		uint32_t mId;
		uint32_t mOffset;
		uint32_t mSourceAddress;
	};

	struct BlockElement {
		enum class Type {
			Text,
			RawByte
		};

		Type mType;

		std::u32string mTextContent;
		uint8_t mRawByte;
	};

	struct Block {
		std::list<Pointer *> mPointerList;
		std::list<BlockElement *> mElementList;
	};

private:
	std::vector<Block *> mBlockList;

public:
	Text();
	virtual ~Text();

	int addBlock(Block *block);

	size_t getBlockCount() const;
	Block *getBlock(size_t index);

	int encode(
		const Table &table,
		uint8_t **outRawText,
		size_t *outRawTextSize,
		std::list<Pointer *> *pointerList) const;

	int decode(
		const uint8_t *rawText,
		size_t rawTextSize,
		const Table &table,
		const std::vector<Pointer *> &pointerList);
};

} // ht

#endif // !__HT_TEXT_HPP__
