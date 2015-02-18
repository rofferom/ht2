#ifndef __HT_OUTPUT_HPP__
#define __HT_OUTPUT_HPP__

namespace ht {

class IOutput {
public:
	virtual ~IOutput() {}

	virtual int writeBuffer(off64_t pos, const Buffer *buffer) = 0;
	virtual int writeRawBuffer(off64_t pos, const uint8_t *data, size_t len) = 0;
};

} // namespace ht

#endif // !__HT_OUTPUT_HPP__
