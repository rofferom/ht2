#ifndef __HT_BUFFER_HPP__
#define __HT_BUFFER_HPP__

#include <stdint.h>

namespace ht {

class Buffer {
private:
	uint8_t *mData;
	size_t mSize;
	size_t mUsedSize;

public:
	Buffer();
	Buffer(const uint8_t *data, size_t len);
	~Buffer();

	size_t getSize() const;
	const uint8_t *getData() const;

	int append(const uint8_t *data, size_t len);
};

} // namespace ht

#endif // !__HT_BUFFER_HPP__
