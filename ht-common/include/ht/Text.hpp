#ifndef __HT_TEXT_HPP__
#define __HT_TEXT_HPP__

#include <string>
#include <vector>
#include <ht/Buffer.hpp>
#include <ht/Pointer.hpp>
#include <ht/Table.hpp>

namespace ht {

class Text {
public:
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
		std::vector<Pointer> mPointerList;
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
		PointerTable *pointerList) const;

	int decode(
		const Buffer &buffer,
		const Table &table,
		const PointerTable &pointerList);
};

} // ht

#endif // !__HT_TEXT_HPP__
