#ifndef __HT_LOG_HPP__
#define __HT_LOG_HPP__

namespace ht {

class Log {
public:
	enum class Level {
		Error = 0,
		Warning,
		Info,
		Debug
	};

	typedef int (*LogCb)(Level priority, const char32_t *tag, const char32_t *format, va_list ap);

private:
	static LogCb sCb;
	static Level sLogLevel;

	static int log(Level level, const char32_t *tag, const char32_t *format, va_list args);

public:
	HTAPI static int setCallback(LogCb cb);
	HTAPI static void setLogLevel(Level level);

	HTAPI static int d(const char32_t *tag, const char32_t *format, ...);
	HTAPI static int i(const char32_t *tag, const char32_t *format, ...);
	HTAPI static int w(const char32_t *tag, const char32_t *format, ...);
	HTAPI static int e(const char32_t *tag, const char32_t *format, ...);
};

} // ht

#endif // !__HT_LOG_HPP__
