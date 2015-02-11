#include <string.h>
#include <errno.h>
#include <iconv.h>
#include <ht/Log.hpp>
#include <ht/CharsetConverter.hpp>

static const char32_t *TAG = U"CharsetConverter";

namespace ht {

struct CharsetConverter {
	iconv_t cd;
	struct CharsetConverterCb cb;
};

struct CharsetConverter *charsetConverterCreate()
{
	struct CharsetConverter *self;

	self = (struct CharsetConverter *) malloc(sizeof(struct CharsetConverter));
	if (self != NULL) {
		self->cd = NULL;
		memset(&self->cb, 0, sizeof(self->cb));
	}

	return self;
}

void charsetConverterDestroy(struct CharsetConverter *self)
{
	free(self);
}

int charsetConverterOpen(
	struct CharsetConverter *self,
	const char* tocode,
	const char* fromcode,
	const struct CharsetConverterCb *cb)
{
	int res;

	if (!self || !tocode || !fromcode || !cb) {
		return -EINVAL;
	}

	self->cd = iconv_open(tocode, fromcode);
	if (self->cd == (iconv_t) -1) {
		res = -errno;
		goto error;
	}

	self->cb = *cb;

	return 0;

error:
	return res;
}

int charsetConverterClose(struct CharsetConverter *self)
{
	if (!self) {
		return -EINVAL;;
	}

	iconv_close(self->cd);
	self->cd = NULL;

	memset(&self->cb, 0, sizeof(self->cb));

	return 0;
}

int charsetConverterInput(
	struct CharsetConverter *self,
	const void *rawContent,
	size_t rawContentSize)
{
	const size_t BUFFSIZE = 1024;
	char convertedContent[BUFFSIZE];
	size_t convertedContentRemainingSize;
	char *convertedContentPos;
	size_t rawContentRemainingSize;
	const char *rawContentPos;
	size_t iconvRes;
	int res = 0;

	rawContentPos = (const char *) rawContent;
	rawContentRemainingSize = rawContentSize;

	while (rawContentRemainingSize > 0) {
		convertedContentPos = convertedContent;
		convertedContentRemainingSize = BUFFSIZE;

		iconvRes = iconv(
			self->cd,
#if defined(__GLIBC__) || defined(__APPLE__)
			(char **) &rawContentPos,
#else
			&rawContentPos,
#endif // __GLIBC__
			&rawContentRemainingSize,
			&convertedContentPos,
			&convertedContentRemainingSize);
		if (iconvRes != (size_t) -1) {
			// Conversion ok
			self->cb.output(
				convertedContent,
				convertedContentPos - convertedContent,
				self->cb.userdata);
		} else if (errno == E2BIG) {
			// Convertion ok but not enough space to convert all the input
			self->cb.output(
				convertedContent,
				convertedContentPos - convertedContent,
				self->cb.userdata);
		} else if (errno == EILSEQ) {
			// Partial conversion
			if (convertedContentRemainingSize < BUFFSIZE) {
				self->cb.output(
					convertedContent,
					convertedContentPos - convertedContent,
					self->cb.userdata);
			}

			rawContentPos++;
			rawContentRemainingSize--;

			if (self->cb.invalid_sequence != NULL) {
				self->cb.invalid_sequence(self->cb.userdata);
			} else {
				Log::e(TAG, U"Invalid sequence");
			}
		} else {
			Log::e(TAG, U"Unhandled error");
		}
	}

	return res;
}

} // ht
