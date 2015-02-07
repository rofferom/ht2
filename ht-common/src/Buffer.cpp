#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ht/Buffer.hpp>

namespace ht {

Buffer::Buffer()
{
	mData = NULL;
	mCapacity = 0;
	mSize = 0;
}

Buffer::Buffer(const uint8_t *data, size_t len)
{
	mData = (uint8_t *) malloc(len);
	memcpy(mData, data, len);

	mCapacity = len;
	mSize = len;
}

Buffer::~Buffer()
{
	free(mData);
}

void Buffer::clear()
{
	free(mData);
	mCapacity = 0;
	mSize = 0;
}

size_t Buffer::getCapacity() const
{
	return mCapacity;
}

int Buffer::setCapacity(size_t capacity)
{
	mData = (uint8_t *) realloc(mData, capacity);
	if (mData == NULL) {
		return -ENOMEM;
	}

	mCapacity = capacity;

	return 0;
}

size_t Buffer::getSize() const
{
	return mSize;
}

void Buffer::setSize(size_t size)
{
	mSize = size;
}

const uint8_t *Buffer::getData() const
{
	return mData;
}

uint8_t *Buffer::getData()
{
	return mData;
}

uint8_t Buffer::getByte(size_t pos) const
{
	return mData[pos];
}

void Buffer::setByte(size_t pos, uint8_t value)
{
	mData[pos] = value;
}

int Buffer::append(const uint8_t *data, size_t len)
{
	if (len + mSize > mCapacity) {
		int res;

		if (mCapacity == 0) {
			res = setCapacity(1024);
		} else {
			res = setCapacity(mCapacity * 2);
		}

		if (res < 0) {
			return res;
		}
	}

	memcpy(mData + mSize, data, len);
	mSize += len;

	return len;
}

} // namespace ht
