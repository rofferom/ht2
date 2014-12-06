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

		typedef int (*LogCb)(Priority priority, const char *tag, const char *format, va_list ap);

private:
		static LogCb sCb;

public:
		static int setCallback(LogCb cb);

		static int d(const char *tag, const char *format, ...);
		static int i(const char *tag, const char *format, ...);
		static int w(const char *tag, const char *format, ...);
		static int e(const char *tag, const char *format, ...);
};

} // ht

#endif // !__HT_LOG_HPP__
