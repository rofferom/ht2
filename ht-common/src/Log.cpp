#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <vfprintf32.h>
#include "ht-private.hpp"

namespace {

static const char32_t *strLogPriority[] = {
	U"D",
	U"I",
	U"W",
	U"E",
};

struct StdoutLogger {
	ht::CharsetConverter::Cb mOutputCb;
	struct OutputStream mStdoutCb;

	StdoutLogger();
};

StdoutLogger stdoutLogger;

size_t writeStdout(const char32_t *s, size_t size, void *userdata)
{
	ht::SysEnv::toSysEnv(s, size * sizeof(char32_t), stdoutLogger.mOutputCb);

	return size;
}

StdoutLogger::StdoutLogger()
{
	mOutputCb.output = [] (const void *buff, size_t size) -> int {
		return fwrite(buff, 1, size, stdout);
	};

	mStdoutCb.write = writeStdout;
	mStdoutCb.userdata = NULL;
}

int logStdout(ht::Log::Priority priority, const char32_t *tag, const char32_t *format, va_list ap)
{
	int res;

	res = fprintf32(&stdoutLogger.mStdoutCb, U"[%s][%s] ", strLogPriority[priority], tag);
	res += vfprintf32(&stdoutLogger.mStdoutCb, format, ap);
	res += fprintf32(&stdoutLogger.mStdoutCb, U"\n");

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

int Log::d(const char32_t *tag, const char32_t *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = sCb(LOG_PRIORITY_DEBUG, tag, format, args);
	va_end(args);

	return res;
}

int Log::i(const char32_t *tag, const char32_t *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = sCb(LOG_PRIORITY_INFO, tag, format, args);
	va_end(args);

	return res;
}

int Log::w(const char32_t *tag, const char32_t *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = sCb(LOG_PRIORITY_WARNING, tag, format, args);
	va_end(args);

	return res;
}

int Log::e(const char32_t *tag, const char32_t *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = sCb(LOG_PRIORITY_ERROR, tag, format, args);
	va_end(args);

	return res;
}

} // namespace ht

