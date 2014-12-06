#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>
#include <ht/Log.hpp>
#include <ht/Utils.hpp>
#include <ht/TextFile.hpp>

#define ALLOCATION_STEP 4096

static const char *TAG = "TextFile";

namespace {

size_t removeCr(char *content, size_t size)
{
	size_t res = 0;

	for (size_t i = 0 ; i < size ; i++) {
		// Skip Carriage Return
		if (content[i] == '\r') {
			continue;
		}

		// Avoid copy if no carriage return has been found yet
		if (res < size) {
			content[res] = content[i];
		}

		res++;
	}

	return res;
}

int convertToUnicode(
	const char *encoding,
	char *rawContent,
	size_t rawContentSize,
	char32_t **outContent,
	size_t *outContentSize)
{
	iconv_t cd = (iconv_t) -1;
	char *convertedContent = NULL;
	size_t convertedContentSize;
	size_t convertedContentRemainingSize;
	size_t rawContentRemainingSize;
	size_t iconvRes;
	char *rawContentPos;
	char *convertedContentPos;
	int res;

	if (encoding == NULL || rawContent == NULL || outContent == NULL || outContentSize == NULL) {
		return -EINVAL;
	}

	cd = iconv_open("UTF-32LE", encoding);
	if (cd == (iconv_t) -1) {
		res = -errno;
		goto error;
	}

	convertedContentSize = rawContentSize;
	convertedContent = (char *) malloc(convertedContentSize);
	if (convertedContent == NULL) {
		res = -ENOMEM;
		goto error;
	}

	rawContentPos = rawContent;
	rawContentRemainingSize = rawContentSize;

	convertedContentPos = convertedContent;
	convertedContentRemainingSize = convertedContentSize;
	while (rawContentRemainingSize > 0) {
		iconvRes = iconv(
			cd,
			&rawContentPos,
			&rawContentRemainingSize,
			&convertedContentPos,
			&convertedContentRemainingSize);
		if (iconvRes != (size_t) -1) {
			// Conversion ok
			res = 0;
		} else if (errno == E2BIG) {
			size_t offset = convertedContentPos - convertedContent;

			convertedContentSize += ALLOCATION_STEP;

			convertedContent = (char *) realloc(convertedContent, convertedContentSize);
			if (convertedContent == NULL) {
				res = -ENOMEM;
				break;
			}

			convertedContentPos = convertedContent + offset;
			convertedContentRemainingSize += ALLOCATION_STEP;
		} else if (errno == EILSEQ) {
			ht::Log::i(TAG, "Invalid sequence");
			res = -EINVAL;
			break;
		}
	}

	iconv_close(cd);

	if (res == 0) {
		*outContent = (char32_t *) convertedContent;
		*outContentSize = (convertedContentSize - convertedContentRemainingSize) / sizeof(char32_t);
	}

	return res;

error:
	free(convertedContent);
	iconv_close(cd);

	return res;
}

} // anonymous namespace

namespace ht {

int TextFile::load(const char *path, const char *encoding, char32_t **outContent, size_t *outSize)
{
	char *rawContent = NULL;
	size_t rawContentSize;
	int res;

	if (outContent == NULL || outSize == NULL) {
		return -EINVAL;
	}

	// Load file content
	res = loadFile(path, &rawContent, &rawContentSize);
	if (res < 0) {
		res = -errno;
		goto error;
	}

	// Remove carriage return and convert content
	rawContentSize = removeCr(rawContent, rawContentSize);
	res = convertToUnicode(encoding, rawContent, rawContentSize, outContent, outSize);
	free(rawContent);

	return res;

error:
	free(rawContent);

	return res;
}

void TextFile::clear(char32_t *content)
{
	free(content);
}

} // ht
