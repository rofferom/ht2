#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ht/Buffer.hpp>

namespace ht {

Buffer::Buffer()
{
	mData = NULL;
	mSize = 0;
	mUsedSize = 0;
}

Buffer::Buffer(const uint8_t *data, size_t len)
{
	mData = (uint8_t *) malloc(len);
	memcpy(mData, data, len);

	mSize = len;
	mUsedSize = len;
}

Buffer::~Buffer()
{
}

size_t Buffer::getSize() const
{
	return mUsedSize;
}

const uint8_t *Buffer::getData() const
{
	return mData;
}

int Buffer::append(const uint8_t *data, size_t len)
{
	if (len + mUsedSize > mSize) {
		if (mSize == 0) {
			mSize = 1024;
		} else {
			mSize *= 2;
		}

		mData = (uint8_t *) realloc(mData, mSize);
		if (mData == NULL) {
			return -ENOMEM;
		}
	}

	memcpy(mData + mUsedSize, data, len);
	mUsedSize += len;

	return len;
}

} // namespace ht
