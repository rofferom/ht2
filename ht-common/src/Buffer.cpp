#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ht-private.hpp"

namespace ht {

Buffer::Buffer()
{
	mData = NULL;
	mCapacity = 0;
	mSize = 0;
	mPos = 0;
}

Buffer::Buffer(const uint8_t *data, size_t len)
{
	mData = (uint8_t *) malloc(len);
	memcpy(mData, data, len);

	mCapacity = len;
	mSize = len;
	mPos = 0;
}

Buffer::~Buffer()
{
	free(mData);
}

void Buffer::clear()
{
	free(mData);
	mData = NULL;
	mCapacity = 0;
	mSize = 0;
	mPos = 0;
}

int Buffer::seek(size_t pos)
{
	if (pos >= mSize)
		return -EINVAL;

	mPos = pos;
	return 0;
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
	return mSize - mPos;
}

int Buffer::setSize(size_t size)
{
	if (size > mCapacity)
		return -EINVAL;

	mSize = size;
	return 0;
}

const uint8_t *Buffer::getData() const
{
	return mData + mPos;
}

uint8_t *Buffer::getData()
{
	return mData + mPos;
}

uint8_t Buffer::getByte(size_t pos) const
{
	return mData[pos + mPos];
}

void Buffer::setByte(size_t pos, uint8_t value)
{
	mData[pos + mPos] = value;
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
