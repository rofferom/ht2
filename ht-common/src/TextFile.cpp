#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ht/Log.hpp>
#include <ht/CharsetConverter.hpp>
#include <ht/Utils.hpp>
#include <ht/TextFile.hpp>

#define ALLOCATION_STEP 4096

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

struct FileConverter {
	uint8_t *buffer;
	size_t size;
	size_t usedSize;
};

int inputConvertedData(const void *buff, size_t size, void *userdata)
{
	FileConverter *fileConverter = (FileConverter *) userdata;

	if (fileConverter->size - fileConverter->usedSize < size) {
		if (ALLOCATION_STEP < size) {
			fileConverter->size += size;
		} else {
			fileConverter->size += ALLOCATION_STEP;
		}

		fileConverter->buffer = (uint8_t *) realloc(fileConverter->buffer, fileConverter->size);
		if (fileConverter->buffer == NULL) {
			return -ENOMEM;
		}
	}

	memcpy(fileConverter->buffer + fileConverter->usedSize, buff, size);
	fileConverter->usedSize += size;

	return 0;
}

int convertToUnicode(
	const char *encoding,
	char *rawContent,
	size_t rawContentSize,
	char32_t **outContent,
	size_t *outContentSize)
{
	struct ht::CharsetConverter *converter;
	struct ht::CharsetConverterCb cb;
	FileConverter fileConverter;
	int res;

	fileConverter.buffer = NULL;
	fileConverter.size = 0;
	fileConverter.usedSize = 0;

	cb.output = inputConvertedData;
	cb.userdata = &fileConverter;

	converter = ht::charsetConverterCreate();
	if (converter == NULL) {
		return -ENOMEM;
	}

	res = ht::charsetConverterOpen(converter, "UTF-32LE", encoding, &cb);
	if (res < 0) {
		goto destroy;
	}

	res = ht::charsetConverterInput(converter, rawContent, rawContentSize);
	if (res == 0) {
		*outContent = (char32_t *) fileConverter.buffer;
		*outContentSize = fileConverter.usedSize / sizeof(char32_t);
	} else {
		free(fileConverter.buffer);
	}

	ht::charsetConverterClose(converter);
	ht::charsetConverterDestroy(converter);

	return 0;

destroy:
	ht::charsetConverterDestroy(converter);

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
