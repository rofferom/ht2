#include <errno.h>
#include "ht-private.hpp"

namespace ht {

File::File()
{
	mFile = NULL;
}

File::~File()
{
	if (mFile) {
		fclose(mFile);
	}
}

int File::open(const char *path, const char *mode)
{
	if (!path || !mode) {
		return -EINVAL;
	}

	mFile = fopen64(path, mode);
	if (!mFile) {
		return -EINVAL;
	}

	return 0;
}

int File::close()
{
	if (!mFile) {
		return -EPERM;
	}

	fclose(mFile);
	mFile = NULL;

	return 0;
}

int File::writeBuffer(off64_t pos, const Buffer *buffer)
{
	return writeRawBuffer(pos, buffer->getData(), buffer->getSize());
}

int File::writeRawBuffer(off64_t pos, const uint8_t *data, size_t len)
{
	int res;

	if (!mFile) {
		return -EPERM;
	} else if (!data) {
		return -EINVAL;
	} else if (len == 0) {
		return 0;
	}

	res = fseeko64(mFile, pos, SEEK_SET);
	if (res == 0) {
		res = fwrite(data, 1, len, mFile);
	} else {
		res = -errno;
	}

	return res;
}

int File::readBuffer(off64_t pos, Buffer *buffer, size_t len)
{
	uint8_t *data;
	int res;

	if (!buffer) {
		return -EINVAL;
	} else if (len == 0) {
		return 0;
	}

	res = buffer->setCapacity(len);
	if (res < 0) {
		return res;
	}

	data = buffer->getData();
	res = readRawBuffer(pos, data, len);
	if (res > 0) {
		buffer->setSize(len);
	}

	return res;
}

int File::readRawBuffer(off64_t pos, uint8_t *data, size_t len)
{
	int res;

	if (!mFile) {
		return -EPERM;
	} else if (!data) {
		return -EINVAL;
	} else if (len == 0) {
		return 0;
	}

	res = fseeko64(mFile, pos, SEEK_SET);
	if (res == 0) {
		res = fread(data, 1, len, mFile);
	} else {
		res = -errno;
	}

	return res;
}

} // namespace ht
