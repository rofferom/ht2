#ifndef __HT_FILE_HPP__
#define __HT_FILE_HPP__

#include <stdint.h>
#include <ht/IOutput.hpp>
#include <ht/IInput.hpp>

namespace ht {

class File : public IOutput, public IInput {
private:
	FILE *mFile;

public:
	File();
	virtual ~File();

	int open(const char *path, const char *mode);
	int close();

	virtual int writeBuffer(off64_t pos, const Buffer *buffer) override;
	virtual int writeRawBuffer(off64_t pos, const uint8_t *data, size_t len) override;

	virtual int readBuffer(off64_t pos, Buffer *buffer, size_t len) override;
	virtual int readRawBuffer(off64_t pos, uint8_t *data, size_t len) override;
};

} // namespace ht

#endif // !__HT_FILE_HPP__
