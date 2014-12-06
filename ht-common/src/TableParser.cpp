#include <errno.h>
#include <ht/Log.hpp>
#include <ht/Table.hpp>
#include <ht/TableParser.hpp>
#include <ht/TextFile.hpp>

static const char *TAG = "TableParser";

namespace {

char32_t *strchr32(char32_t *s, size_t size, char32_t c)
{
	for (size_t i = 0 ; i < size ; i++) {
		if (s[i] == c) {
			return s + i;
		}
	}

	return NULL;
}

bool isHexChar(char32_t c)
{
	return ((c >= U'A' && c <= U'F') || (c >= U'a' && c <= U'f') || (c >= U'0' && c <= U'9'));
}

int parseKey(char32_t *strKey, size_t strKeySize, uint8_t **outBuffer, size_t *outSize)
{
	uint8_t *key;
	size_t keySize;
	char tmp[3]; // 2 digit + '\0'
	size_t strKeyIt;
	int keyIt;
	int res;

	res = 0;

	/**
	 * Check that strKey is only composed of hex chars.
	 * If it is the case, cast from char32_t to char32 is allowed because ASCII chars have the
	 * same value in unicode.
	 */
	for (size_t i = 0 ; i < strKeySize ; i++) {
		if (isHexChar(strKey[i]) == false) {
			res = -EINVAL;
			break;
		}
	}

	// Invalid Key
	if (res < 0) {
		goto error;
	}

	keySize = strKeySize / 2 + strKeySize % 2; // strKeySize can be odd

	key = (uint8_t *) malloc(keySize * sizeof(uint8_t));
	if (key == NULL) {
		res = -ENOMEM;
		goto error;
	}

	// Extract start of the key
	if (strKeySize % 2 == 1) {
		tmp[0] = (char) strKey[0];
		tmp[1] = '\0';

		key[0] = strtol(tmp, NULL, 16);

		keyIt = 1;
		strKeyIt = 1;
	} else {
		keyIt = 0;
		strKeyIt = 0;
	}

	// Fill the key array

	while (strKeyIt < strKeySize) {
		tmp[0] = (char) strKey[strKeyIt];
		tmp[1] = (char) strKey[strKeyIt + 1];
		tmp[2] = '\0';

		key[keyIt] = strtol(tmp, NULL, 16);

		strKeyIt += 2;
		keyIt++;
	}

	*outBuffer = key;
	*outSize = keySize;

	return 0;

error:
	return res;
}

int parseLine(int lineNumber, char32_t *line, size_t lineSize, const ht::TableParserNewEntryCb &cb)
{
	char32_t *separator;
	std::u32string value;
	uint8_t *key = NULL;
	size_t keySize;
	size_t valueSize;
	int res;

	ht::Log::d(TAG, "Found new line %d of size %d", lineNumber, lineSize);

	// Ignore empty lines
	if (lineSize == 0) {
		return 0;
	}

	// Get line separator (=)
	separator = strchr32(line, lineSize, U'=');
	if (separator == NULL) {
		ht::Log::e(TAG, "Line %d is invalid", lineNumber);
		res = -EINVAL;
		goto error;
	} else if (line == separator) {
		ht::Log::e(TAG, "Line %d has no key", lineNumber);
		res = -EINVAL;
		goto error;
	} else if ((size_t) (line - separator + 1) == lineSize) {
		ht::Log::e(TAG, "Line %d has no value", lineNumber);
		res = -EINVAL;
		goto error;
	}

	// Extract key
	res = parseKey(line, separator - line, &key, &keySize);
	if (res < 0) {
		ht::Log::e(TAG, "Line %d has an invalid key", lineNumber);
		res = -EINVAL;
		goto error;
	}

	// Extract value
	valueSize = lineSize - (separator - line + 1);
	if (valueSize == 0) {
		ht::Log::e(TAG, "Line %d has an empty value", lineNumber);
		res = -EINVAL;
		goto error;
	}

	value.assign(separator + 1, valueSize);

	// Notify new key,value pair
	cb(key, keySize, value);

	free(key);

	return 0;

error:
	free(key);

	return res;
}

int parseTableContent(char32_t *content, size_t contentSize, const ht::TableParserNewEntryCb &cb)
{
	char32_t *position;
	char32_t *eol;
	size_t size;
	int res = 0;
	int lineNumber = 1;

	position = content;
	size = contentSize;

	while (size > 0) {
		// Find next end of line
		eol = strchr32(position, size, U'\n');

		// We are at the last line
		if (eol == NULL) {
			res = parseLine(lineNumber, position, size, cb);
			break;
		} else {
			res = parseLine(lineNumber, position, eol - position, cb);
			if (res < 0) {
				break;
			}

			// Move to start of next line
			size -= eol - position + 1;
			position = eol + 1;
			lineNumber++;
		}
	}

	return res;
}

}

namespace ht {

int parseTable(const char *path, const char *encoding, const TableParserNewEntryCb &cb)
{
	char32_t *content;
	size_t contentSize;
	int res;

	if (!path|| !encoding || !cb) {
		return -EINVAL;
	}

	res = TextFile::load(path, encoding, &content, &contentSize);
	if (res < 0) {
		goto error;
	}

	res = parseTableContent(content, contentSize, cb);

	TextFile::clear(content);

	return res;

error:
	return res;

}

} // ht

