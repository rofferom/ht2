#ifndef __HT_POINTER_HPP__
#define __HT_POINTER_HPP__

namespace ht {

enum class PointerEndianness {
	Little = 0,
	Big
};

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

class PointerTable {
private:
	std::vector<Pointer> mPointers;

private:
	void readLittleEndian(const uint8_t *in, size_t len, uint32_t *value);
	void writeLittleEndian(uint8_t *out, size_t len, uint32_t value);

	void readBigEndian(const uint8_t *in, size_t len, uint32_t *value);
	void writeBigEndian(uint8_t *out, size_t len, uint32_t value);

public:
	HTAPI PointerTable();
	HTAPI ~PointerTable();

	HTAPI int add(const Pointer &pointer);
	HTAPI int add(const Pointer &&pointer);
	HTAPI Pointer *getPointer(size_t index);
	HTAPI const Pointer *getPointer(size_t index) const;
	HTAPI size_t getCount() const;

	HTAPI int sort(std::function<bool(const Pointer &p1, const Pointer &p2)> cb);

	HTAPI int updateSourceAddress(std::function<uint32_t(uint32_t)> cb);
	HTAPI int updateOffset(std::function<uint32_t(uint32_t)> cb);

	HTAPI int write(IOutput *out, size_t width, PointerEndianness endianness);
	HTAPI int read(IInput *input, off64_t pos, uint32_t count, size_t width, PointerEndianness endianness);
};

} // namespace ht

#endif // !__HT_POINTER_HPP__
