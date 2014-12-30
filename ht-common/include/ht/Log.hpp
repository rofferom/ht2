#ifndef __HT_LOG_HPP__
#define __HT_LOG_HPP__

#include <stdarg.h>

namespace ht {

class Log {
public:
	enum Priority {
		LOG_PRIORITY_DEBUG = 0,
		LOG_PRIORITY_INFO,
		LOG_PRIORITY_WARNING,
		LOG_PRIORITY_ERROR,
		LOG_PRIORITY_COUNT,
	};

	typedef int (*LogCb)(Priority priority, const char32_t *tag, const char32_t *format, va_list ap);

private:
	static LogCb sCb;

public:
	static int setCallback(LogCb cb);

	static int d(const char32_t *tag, const char32_t *format, ...);
	static int i(const char32_t *tag, const char32_t *format, ...);
	static int w(const char32_t *tag, const char32_t *format, ...);
	static int e(const char32_t *tag, const char32_t *format, ...);
};

} // ht

#endif // !__HT_LOG_HPP__
