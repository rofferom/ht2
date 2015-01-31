#include <errno.h>
#include <ht/File.hpp>

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

	mFile = fopen(path, mode);
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

} // namespace ht
