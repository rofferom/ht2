#ifndef __HT_INPUT_HPP__
#define __HT_INPUT_HPP__

#include <stdio.h>
#include <stdint.h>
#include <ht/Buffer.hpp>

namespace ht {

class IInput {
public:
	virtual ~IInput() {}

	virtual int readBuffer(off64_t pos, Buffer *buffer, size_t len) = 0;
	virtual int readRawBuffer(off64_t pos, uint8_t *data, size_t len) = 0;
};

} // namespace ht

#endif // !__HT_INPUT_HPP__
