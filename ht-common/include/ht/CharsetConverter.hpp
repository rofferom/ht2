#ifndef __HT_CHARSETCONVERTER_HPP__
#define __HT_CHARSETCONVERTER_HPP__

#include <stdlib.h>

namespace ht {

struct CharsetConverter;

struct CharsetConverterCb {
	int (*output)(const void *buff, size_t size, void *userdata);
	int (*invalid_sequence)(void *userdata);
	void *userdata;
};

struct CharsetConverter *charsetConverterCreate();

void charsetConverterDestroy(struct CharsetConverter *self);

int charsetConverterOpen(
	struct CharsetConverter *self,
	const char* tocode,
	const char* fromcode,
	const struct CharsetConverterCb *cb);

int charsetConverterClose(struct CharsetConverter *self);

int charsetConverterInput(
	struct CharsetConverter *self,
	const void *rawContent,
	size_t rawContentSize);

} // ht

#endif // !__HT_CHARSETCONVERTER_HPP__
