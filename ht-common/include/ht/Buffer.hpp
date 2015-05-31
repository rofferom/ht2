#ifndef __HT_BUFFER_HPP__
#define __HT_BUFFER_HPP__

namespace ht {

class Buffer {
private:
	uint8_t *mData;
	size_t mCapacity;
	size_t mSize;
	size_t mPos;

public:
	HTAPI Buffer();
	HTAPI Buffer(const uint8_t *data, size_t len);
	HTAPI ~Buffer();

	HTAPI void clear();

	HTAPI int seek(size_t pos);

	HTAPI size_t getCapacity() const;
	HTAPI int setCapacity(size_t capacity);

	HTAPI size_t getSize() const;
	HTAPI int setSize(size_t size);

	HTAPI const uint8_t *getData() const;
	HTAPI uint8_t *getData();

	HTAPI uint8_t getByte(size_t pos) const;
	HTAPI void setByte(size_t pos, uint8_t value);

	HTAPI int append(const uint8_t *data, size_t len);
};

} // namespace ht

#endif // !__HT_BUFFER_HPP__
