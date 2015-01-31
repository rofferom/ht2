#ifndef __HT_POINTER_HPP__
#define __HT_POINTER_HPP__

#include <stdint.h>
#include <vector>
#include <functional>
#include <ht/IOutput.hpp>

namespace ht {

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
public:
	enum class Endianness {
		Little = 0,
		Big
	};

private:
	std::vector<Pointer> mPointers;

private:
	void fillLittleEndian(uint8_t *out, size_t len, uint32_t value);
	void fillBigEndian(uint8_t *out, size_t len, uint32_t value);

public:
	PointerTable();
	~PointerTable();

	int add(const Pointer &pointer);
	int add(const Pointer &&pointer);
	Pointer *getPointer(size_t index);
	const Pointer *getPointer(size_t index) const;
	size_t getCount() const;

	int sort(std::function<bool(const Pointer &p1, const Pointer &p2)> cb);

	int updateSourceAddress(std::function<uint32_t(uint32_t)> cb);
	int updateOffset(std::function<uint32_t(uint32_t)> cb);

	int write(IOutput *out, size_t width, Endianness endianness);
};

} // namespace ht

#endif // !__HT_POINTER_HPP__
