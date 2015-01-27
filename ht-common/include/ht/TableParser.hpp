#ifndef __HT_TABLEPARSER_HPP__
#define __HT_TABLEPARSER_HPP__

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <ht/Utils.hpp>
#include <ht/Table.hpp>

namespace ht {

/**
 * const uint8_t *key
 * size_t keyLen
 * const char *value
 */
typedef Callback<void(const uint8_t *, size_t, const std::u32string &)> TableParserNewEntryCb;

int parseKey(const char32_t *strKey, size_t strKeySize, uint8_t **outBuffer, size_t *outSize);

int encodeKey(const Table::Key *key, std::u32string *out);
int encodeKey(const uint8_t *key, size_t keySize, std::u32string *out);

int parseTable(const char *path, const char *encoding, const TableParserNewEntryCb::Type &cb);

} // ht

#endif // !__HT_TABLEPARSER_HPP__
