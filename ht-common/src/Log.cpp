#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <vfprintf32.h>
#include "ht-private.hpp"

namespace {

static const char32_t *strLogPriority[] = {
	U"E",
	U"W",
	U"I",
	U"D",
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

int logStdout(ht::Log::Level priority, const char32_t *tag, const char32_t *format, va_list ap)
{
	int res;

	res = fprintf32(&stdoutLogger.mStdoutCb, U"[%s][%s] ", strLogPriority[(int) priority], tag);
	res += vfprintf32(&stdoutLogger.mStdoutCb, format, ap);
	res += fprintf32(&stdoutLogger.mStdoutCb, U"\n");

	return res;
}

} // anonymous namespace

namespace ht {

Log::LogCb Log::sCb = logStdout;
Log::Level Log::sLogLevel = Log::Level::Info;

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

void Log::setLogLevel(Level level)
{
	sLogLevel = level;
}

int Log::log(Level level, const char32_t *tag, const char32_t *format, va_list args)
{
	int res;

	if (level <= sLogLevel) {
		res = sCb(level, tag, format, args);
	} else {
		res = 0;
	}

	return res;
}

int Log::d(const char32_t *tag, const char32_t *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = log(Log::Level::Debug, tag, format, args);
	va_end(args);

	return res;
}

int Log::i(const char32_t *tag, const char32_t *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = log(Log::Level::Info, tag, format, args);
	va_end(args);

	return res;
}

int Log::w(const char32_t *tag, const char32_t *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = log(Log::Level::Warning, tag, format, args);
	va_end(args);

	return res;
}

int Log::e(const char32_t *tag, const char32_t *format, ...)
{
	va_list args;
	int res;

	va_start(args, format);
	res = log(Log::Level::Error, tag, format, args);
	va_end(args);

	return res;
}

} // namespace ht

