#ifndef __HT_TABLEPARSER_HPP__
#define __HT_TABLEPARSER_HPP__

namespace ht {

/**
 * const uint8_t *key
 * size_t keyLen
 * const char *value
 */
typedef Callback<void(const uint8_t *, size_t, const std::u32string &)> TableParserNewEntryCb;

HTAPI int parseKey(const char32_t *strKey, size_t strKeySize, uint8_t **outBuffer, size_t *outSize);

HTAPI int encodeKey(const Table::Key *key, std::u32string *out);
HTAPI int encodeKey(const uint8_t *key, size_t keySize, std::u32string *out);

HTAPI int parseTable(const char *path, const char *encoding, const TableParserNewEntryCb::Type &cb);

} // ht

#endif // !__HT_TABLEPARSER_HPP__
