#include <errno.h>
#include <algorithm>
#include <ht/Pointer.hpp>

#define MAX_WIDTH 3

namespace ht {

PointerTable::PointerTable()
{
}

PointerTable::~PointerTable()
{
}

int PointerTable::add(const Pointer &pointer)
{
	mPointers.push_back(pointer);

	return 0;
}

int PointerTable::add(const Pointer &&pointer)
{
	mPointers.push_back(pointer);

	return 0;
}

Pointer *PointerTable::getPointer(size_t index)
{
	if (index >= mPointers.size()) {
		return NULL;
	}

	return &mPointers[index];
}

const Pointer *PointerTable::getPointer(size_t index) const
{
	if (index >= mPointers.size()) {
		return NULL;
	}

	return &mPointers[index];
}

size_t PointerTable::getCount() const
{
	return mPointers.size();
}

int PointerTable::sort(std::function<bool(const Pointer &p1, const Pointer &p2)> cb)
{
	std::sort(mPointers.begin(), mPointers.end(), cb);

	return 0;
}

int PointerTable::updateSourceAddress(std::function<uint32_t(uint32_t)> cb)
{
	for (auto &p : mPointers) {
		p.mSourceAddress = cb(p.mId);
	}

	return 0;
}

int PointerTable::updateOffset(std::function<uint32_t(uint32_t)> cb)
{
	for (auto &p : mPointers) {
		p.mOffset = cb(p.mOffset);
	}

	return 0;
}

void PointerTable::fillLittleEndian(uint8_t *out, size_t len, uint32_t value)
{
	for (size_t i = 0 ; i < len ; i++) {
		out[i] = (value >> (i * 8)) & 0xFF;
	}
}

void PointerTable::fillBigEndian(uint8_t *out, size_t len, uint32_t value)
{
	for (size_t i = 0 ; i < len ; i++) {
		out[len-i-1] = (value >> (i * 8)) & 0xFF;
	}
}

int PointerTable::write(IOutput *out, size_t width, Endianness endianness)
{
	int res;

	if (!out) {
		return -EINVAL;
	} else if (width == 0) {
		return -EINVAL;
	} else if (width > MAX_WIDTH) {
		return -EINVAL;
	}

	res = 0;
	for (auto &p : mPointers) {
		uint8_t tmp[MAX_WIDTH];

		switch (endianness) {
		case Endianness::Little:
			fillLittleEndian(tmp, width, p.mOffset);
			break;

		case Endianness::Big:
			fillBigEndian(tmp, width, p.mOffset);
			break;

		default:
			break;
		}

		res = out->writeRawBuffer(p.mSourceAddress, tmp, width);
		if (res < 0) {
			break;
		}
	}

	return res;
}

} // namespace ht
