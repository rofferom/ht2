#include <errno.h>
#include <algorithm>
#include "ht-private.hpp"

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

void PointerTable::readLittleEndian(const uint8_t *in, size_t len, uint32_t *value)
{
	uint32_t tmp = 0;

	for (size_t i = 0 ; i < len ; i++) {
		tmp |= in[i] << (i * 8);
	}

	*value = tmp;
}

void PointerTable::writeLittleEndian(uint8_t *out, size_t len, uint32_t value)
{
	for (size_t i = 0 ; i < len ; i++) {
		out[i] = (value >> (i * 8)) & 0xFF;
	}
}

void PointerTable::readBigEndian(const uint8_t *in, size_t len, uint32_t *value)
{
	uint32_t tmp = 0;

	for (size_t i = 0 ; i < len ; i++) {
		tmp |= in[len-i-1] << (i * 8);
	}

	*value = tmp;
}

void PointerTable::writeBigEndian(uint8_t *out, size_t len, uint32_t value)
{
	for (size_t i = 0 ; i < len ; i++) {
		out[len-i-1] = (value >> (i * 8)) & 0xFF;
	}
}

int PointerTable::write(IOutput *out, size_t width, PointerEndianness endianness)
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
		case PointerEndianness::Little:
			writeLittleEndian(tmp, width, p.mOffset);
			break;

		case PointerEndianness::Big:
			writeBigEndian(tmp, width, p.mOffset);
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

int PointerTable::read(
	IInput *input,
	off64_t pos,
	uint32_t count,
	size_t width,
	PointerEndianness endianness)
{
	int res;

	if (!input) {
		return -EINVAL;
	} else if (count == 0) {
		return -EINVAL;
	} else if (width == 0) {
		return -EINVAL;
	} else if (width > MAX_WIDTH) {
		return -EINVAL;
	}

	res = 0;
	for (uint32_t i = 0 ; i < count ; i++) {
		Pointer p;
		uint8_t tmp[MAX_WIDTH];

		res = input->readRawBuffer(pos + i * width, tmp, width);
		if (res < 0) {
			break;
		}

		p.mId = i + 1;
		p.mSourceAddress = pos + i * width;

		switch (endianness) {
		case PointerEndianness::Little:
			readLittleEndian(tmp, width, &p.mOffset);
			break;

		case PointerEndianness::Big:
			readBigEndian(tmp, width, &p.mOffset);
			break;

		default:
			break;
		}

		mPointers.push_back(p);
	}

	return res;
}

} // namespace ht
