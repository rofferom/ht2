#ifndef __HT_LEGACY_TABLELOADER_HPP__
#define __HT_LEGACY_TABLELOADER_HPP__

#include <ht/Table.hpp>
#include <ht/Env.hpp>

namespace htl {

HTAPI int loadTable(const char *tablePath, const char *encoding, ht::Table *table);

} // namespace htl

#endif // !__HT_LEGACY_TABLELOADER_HPP__
