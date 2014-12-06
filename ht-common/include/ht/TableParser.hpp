#ifndef __HT_TABLEPARSER_HPP__
#define __HT_TABLEPARSER_HPP__

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <ht/Callback.hpp>

namespace ht {

/**
 * const uint8_t *key
 * size_t keyLen
 * const char *value
 */
typedef Callback<void(const uint8_t *, size_t, const std::u32string &)> TableParserNewEntryCb;

int parseTable(const char *path, const char *encoding, const TableParserNewEntryCb &cb);

} // ht

#endif // !__HT_TABLEPARSER_HPP__
