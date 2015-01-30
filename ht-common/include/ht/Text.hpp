#ifndef __HT_TEXT_HPP__
#define __HT_TEXT_HPP__

#include <string>
#include <vector>
#include <ht/Buffer.hpp>
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

	typedef std::vector<Pointer *> PointerList;

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
		PointerList mPointerList;
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
		Buffer *buffer,
		PointerList *pointerList) const;

	int decode(
		const Buffer &buffer,
		const Table &table,
		const PointerList &pointerList);
};

} // ht

#endif // !__HT_TEXT_HPP__
