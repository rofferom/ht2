#ifndef __HT_TYPES_HPP__
#define __HT_TYPES_HPP__

#define HTAPI __attribute__((visibility("default")))

#ifdef __APPLE__
#define off64_t off_t
#define fopen64 fopen
#define fseeko64 fseeko
#endif

static_assert(sizeof(off64_t) == 8, "Not 64 bits off64_t");

#endif // !__HT_TYPES_HPP__
