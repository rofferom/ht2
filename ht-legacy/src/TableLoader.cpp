#include <stdlib.h>
#include <string.h>
#include <ht/TableParser.hpp>
#include <ht-legacy/TableLoader.hpp>

struct TableLoader {
	ht::Table *mTable;

	TableLoader(ht::Table *table)
	{
		mTable = table;
	}

	void convertEndOfLines(char32_t *cleanedValue, size_t valueSize)
	{
		char32_t *readPtr = cleanedValue;
		char32_t *writePtr = cleanedValue;

		for (size_t i = 0 ; i < valueSize ;) {
			if (*readPtr != U'\\') {
				// Ignore non \ chars
				*writePtr = *readPtr;
				readPtr++;
				writePtr++;
				i++;
			} else if (i+1 == valueSize) {
				// Ignore \ chars that are at end of line
				*writePtr = *readPtr;
				readPtr++;
				writePtr++;
				i++;
			} else if (readPtr[1] == U'n') {
				// End of line detected
				*writePtr = U'\n';
				readPtr += 2;
				writePtr++;
				i += 2;
			} else {
				*writePtr = *readPtr;
				readPtr++;
				writePtr++;
				i++;
			}
		}

		*writePtr = U'\0';
	}

	void entryFound(const uint8_t *key, size_t keySize, const std::u32string &value)
	{
		char32_t *cleanedValue;
		size_t valueSize = value.size();

		// Get a temp value
		cleanedValue = (char32_t *) malloc((valueSize + 1) * sizeof(char32_t));
		if (cleanedValue == NULL) {
			return;
		}

		memcpy(cleanedValue, value.c_str(), valueSize * sizeof(char32_t));
		cleanedValue[valueSize] = U'\0';

		// Convert end of lines
		convertEndOfLines(cleanedValue, valueSize);

		// Push the value
		mTable->addEntry(key, keySize, cleanedValue);

		free(cleanedValue);
	}
};


namespace htl {

int loadTable(const char *tablePath, const char *encoding, ht::Table *table)
{
	TableLoader loader(table);
	ht::TableParserNewEntryCb cb(&loader, &TableLoader::entryFound);

	return ht::parseTable(tablePath, encoding, cb);
}

} // namespace htl
