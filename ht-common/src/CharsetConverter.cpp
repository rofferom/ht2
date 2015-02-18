#include <string.h>
#include <errno.h>
#include <iconv.h>
#include "ht-private.hpp"

static const char32_t *TAG = U"CharsetConverter";

namespace ht {

class CharsetConverterImpl : public CharsetConverter {
private:
	iconv_t mCd;
	Cb mCb;

public:
	CharsetConverterImpl();
	virtual ~CharsetConverterImpl();

	virtual int open(const char *tocode, const char *fromcode) override;
	virtual int open(const char *tocode, const char *fromcode, const Cb &cb) override;
	virtual int close() override;

	virtual int input(const void *content, size_t contentSize) override;
	virtual int input(const void *content, size_t contentSize, const Cb &cb) override;
};

CharsetConverterImpl::CharsetConverterImpl()
	: CharsetConverter()
{
	mCd = NULL;
}

CharsetConverterImpl::~CharsetConverterImpl()
{
	close();
}

int CharsetConverterImpl::open(const char *tocode, const char *fromcode)
{
	int res;

	if (!tocode || !fromcode) {
		return -EINVAL;
	}

	mCd = iconv_open(tocode, fromcode);
	if (mCd == (iconv_t) -1) {
		res = -errno;
		goto error;
	}

	return 0;

error:
	mCd = NULL;

	return res;
}

int CharsetConverterImpl::open(const char *tocode, const char *fromcode, const Cb &cb)
{
	int res;

	if (!tocode || !fromcode || cb.isValid() == false) {
		return -EINVAL;
	}

	mCd = iconv_open(tocode, fromcode);
	if (mCd == (iconv_t) -1) {
		res = -errno;
		goto error;
	}

	mCb = cb;

	return 0;

error:
	mCd = NULL;

	return res;
}

int CharsetConverterImpl::close()
{
	if (mCd != NULL) {
		iconv_close(mCd);
		mCd = NULL;
	}

	return 0;
}

int CharsetConverterImpl::input(const void *rawContent, size_t rawContentSize)
{
	if (mCb.isValid() == false) {
		return -EINVAL;
	}

	return input(rawContent, rawContentSize, mCb);
}

int CharsetConverterImpl::input(const void *rawContent, size_t rawContentSize, const Cb &cb)
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
			mCd,
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
			cb.output(
				convertedContent,
				convertedContentPos - convertedContent);
		} else if (errno == E2BIG) {
			// Convertion ok but not enough space to convert all the input
			cb.output(
				convertedContent,
				convertedContentPos - convertedContent);
		} else if (errno == EILSEQ) {
			// Partial conversion
			if (convertedContentRemainingSize < BUFFSIZE) {
				cb.output(
					convertedContent,
					convertedContentPos - convertedContent);
			}

			rawContentPos++;
			rawContentRemainingSize--;

			if (cb.invalidSeq) {
				cb.invalidSeq();
			} else {
				Log::e(TAG, U"Invalid sequence");
			}
		} else if (errno == EINVAL) {
			rawContentPos++;
			rawContentRemainingSize--;

			if (cb.invalidSeq) {
				cb.invalidSeq();
			} else {
				Log::e(TAG, U"Invalid sequence");
			}
		} else {
			Log::e(TAG, U"Unhandled error %d", errno);
		}
	}

	return res;
}

CharsetConverter *CharsetConverter::create()
{
	return new CharsetConverterImpl();
}

void CharsetConverter::destroy(CharsetConverter *self)
{
	delete self;
}

} // ht
