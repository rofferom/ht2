#ifndef __HT_UTILS_HPP__
#define __HT_UTILS_HPP__

#include <stdlib.h>
#include <stdint.h>

#define HT_SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))

namespace ht {

int loadFile(const char *path, char **outContent, size_t *outContentSize);
int dumpToFile(const char *path, const uint8_t *buffer, size_t size);

} // ht


#endif // !__HT_UTILS_HPP__
