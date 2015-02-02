#ifndef __HT_BUFFER_HPP__
#define __HT_BUFFER_HPP__

#include <stdint.h>

namespace ht {

class Buffer {
private:
	uint8_t *mData;
	size_t mCapacity;
	size_t mSize;

public:
	Buffer();
	Buffer(const uint8_t *data, size_t len);
	~Buffer();

	void clear();

	size_t getCapacity() const;
	int setCapacity(size_t capacity);

	size_t getSize() const;
	void setSize(size_t size);

	const uint8_t *getData() const;
	uint8_t *getData();

	uint8_t getByte(size_t pos) const;
	void setByte(size_t pos, uint8_t value);

	int append(const uint8_t *data, size_t len);
};

} // namespace ht

#endif // !__HT_BUFFER_HPP__
