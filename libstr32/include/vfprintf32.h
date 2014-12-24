#ifndef __VFPRINTF32_H__
#define __VFPRINTF32_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef __cplusplus
	#ifndef char32_t
		typedef __CHAR32_TYPE__ char32_t;
	#endif // !char32_t
#endif // !__cplusplus

struct OutputStream {
	size_t (*write)(const char32_t *s, size_t size, void *userdata);
	void *userdata;
};

int fprintf32(struct OutputStream *f, const char32_t *fmt, ...);
int vfprintf32(struct OutputStream *f, const char32_t *fmt, va_list ap);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // !__VFPRINTF32_H__
