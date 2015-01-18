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

		Pointer()
		{
			mId = 0;
			mOffset = 0;
			mSourceAddress = 0;
		}

		Pointer(uint32_t id, uint32_t offset, uint32_t sourceAddress)
		{
			mId = id;
			mOffset = offset;
			mSourceAddress = sourceAddress;
		}
	};

	struct BlockElement {
		enum class Type {
			None,
			Text,
			RawByte
		};

		Type mType;

		std::u32string mTextContent;
		uint8_t mRawByte;

		BlockElement()
		{
			mType = Type::None;
			mRawByte = 0;
		}

		BlockElement(Type type, const char32_t *textContent, uint8_t rawByte)
		{
			mType = type;
			mTextContent = textContent;
			mRawByte = rawByte;
		}
	};

	struct Block {
		std::vector<Pointer *> mPointerList;
		std::vector<BlockElement *> mElementList;
	};

private:
	std::vector<Block *> mBlockList;

public:
	Text();
	virtual ~Text();

	int addBlock(Block *block);

	size_t getBlockCount() const;
	Block *getBlock(size_t index);
	const Block *getBlock(size_t index) const;

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
