#ifndef __STRING32_H__
#define __STRING32_H__

#include <stdlib.h>

#ifdef __cpluscplus
extern "C" {
#endif // __cpluscplus

#ifndef char32_t
typedef __CHAR32_TYPE__ char32_t;
#endif

char32_t *memchr32(char32_t *s, char32_t c, size_t n);

#ifdef __cpluscplus
} extern "C"
#endif // __cpluscplus

#endif // !__STRING32_H__
