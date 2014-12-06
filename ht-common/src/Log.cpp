#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ht/Log.hpp>
#include <ht/Utils.hpp>

namespace {

static const char *strLogPriority[] = {
	"D",
	"I",
	"W",
	"E",
};

int logStdout(ht::Log::Priority priority, const char *tag, const char *format, va_list ap)
{
	int res;

	res = printf("[%s][%s] ", strLogPriority[priority], tag);
	res += vprintf(format, ap);
	res += printf("\n");

	return res;
}

} // anonymous namespace

namespace ht {

Log::LogCb Log::sCb = logStdout;

int Log::setCallback(LogCb cb)
{
	int res;

	if (cb == NULL) {
		res = -EINVAL;
	} else {
		sCb = cb;
		res = 0;
	}

	return res;
}

int Log::d(const char *tag, const char *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = sCb(LOG_PRIORITY_DEBUG, tag, format, args);
	va_end(args);

	return res;
}

int Log::i(const char *tag, const char *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = sCb(LOG_PRIORITY_INFO, tag, format, args);
	va_end(args);

	return res;
}

int Log::w(const char *tag, const char *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = sCb(LOG_PRIORITY_WARNING, tag, format, args);
	va_end(args);

	return res;
}

int Log::e(const char *tag, const char *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = sCb(LOG_PRIORITY_ERROR, tag, format, args);
	va_end(args);

	return res;
}

} // namespace ht

