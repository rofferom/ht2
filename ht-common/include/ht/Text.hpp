#ifndef __HT_TEXT_HPP__
#define __HT_TEXT_HPP__

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

private:
	int extractMatchingPointers(
		uint32_t offset,
		size_t start,
		const ht::PointerTable &src,
		std::vector<ht::Pointer> *dest);

public:
	HTAPI Text();
	HTAPI virtual ~Text();

	HTAPI int addBlock(Block *block);

	HTAPI size_t getBlockCount() const;
	HTAPI Block *getBlock(size_t index);
	HTAPI const Block *getBlock(size_t index) const;

	HTAPI int encode(
		const Table &table,
		Buffer *buffer,
		PointerTable *pointerList) const;

	HTAPI int decode(
		const Buffer &buffer,
		const Table &table,
		const PointerTable &pointerList);

	HTAPI static int encodeString(
		const std::u32string &s,
		const ht::Table &table,
		ht::Buffer *buffer);

	HTAPI static int encodeBlock(
		const ht::Text::Block *block,
		const ht::Table &table,
		ht::Buffer *buffer);

	HTAPI static int decodeBuffer(
		const uint8_t *rawText,
		size_t rawTextSize,
		const ht::Table &table,
		ht::Text::Block *block);
};

} // ht

#endif // !__HT_TEXT_HPP__
