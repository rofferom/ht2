#ifndef __HT_CHARSETCONVERTER_HPP__
#define __HT_CHARSETCONVERTER_HPP__

namespace ht {

struct CharsetConverter;

struct CharsetConverterCb {
	int (*output)(const void *buff, size_t size, void *userdata);
	int (*invalid_sequence)(void *userdata);
	void *userdata;

	CharsetConverterCb()
	{
		output = NULL;
		invalid_sequence = NULL;
		userdata = NULL;
	}
};

HTAPI struct CharsetConverter *charsetConverterCreate();

HTAPI void charsetConverterDestroy(struct CharsetConverter *self);

HTAPI int charsetConverterOpen(
	struct CharsetConverter *self,
	const char* tocode,
	const char* fromcode,
	const struct CharsetConverterCb *cb);

HTAPI int charsetConverterClose(struct CharsetConverter *self);

HTAPI int charsetConverterInput(
	struct CharsetConverter *self,
	const void *rawContent,
	size_t rawContentSize);

} // ht

#endif // !__HT_CHARSETCONVERTER_HPP__
