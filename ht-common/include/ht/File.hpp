#ifndef __HT_FILE_HPP__
#define __HT_FILE_HPP__

#include <stdint.h>
#include <ht/IOutput.hpp>
#include <ht/IInput.hpp>
#include <ht/Env.hpp>

namespace ht {

class File : public IOutput, public IInput {
private:
	FILE *mFile;

public:
	HTAPI File();
	HTAPI virtual ~File();

	HTAPI int open(const char *path, const char *mode);
	HTAPI int close();

	HTAPI virtual int writeBuffer(off64_t pos, const Buffer *buffer) override;
	HTAPI virtual int writeRawBuffer(off64_t pos, const uint8_t *data, size_t len) override;

	HTAPI virtual int readBuffer(off64_t pos, Buffer *buffer, size_t len) override;
	HTAPI virtual int readRawBuffer(off64_t pos, uint8_t *data, size_t len) override;
};

} // namespace ht

#endif // !__HT_FILE_HPP__
