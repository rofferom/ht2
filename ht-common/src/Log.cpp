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
	struct ht::CharsetConverter *converter;
	struct OutputStream out;
};

StdoutLogger *stdoutLogger = NULL;

size_t writeStdout(const char32_t *s, size_t size, void *userdata)
{
	StdoutLogger *logger = (StdoutLogger *) userdata;

	ht::charsetConverterInput(logger->converter, s, size * sizeof(char32_t));

	return size;
}

int convertedBuffer(const void *buff, size_t size, void *userdata)
{
	return fwrite(buff, 1, size, stdout);
}

void initStdout()
{
	struct ht::CharsetConverterCb converterCb;
	int res;

	stdoutLogger = new StdoutLogger();
	if (stdoutLogger == NULL) {
		goto error;
	}

	stdoutLogger->converter = ht::charsetConverterCreate();
	if (stdoutLogger->converter == NULL) {
		goto error;
	}

	converterCb.output = convertedBuffer;
	converterCb.invalid_sequence = NULL;
	converterCb.userdata = stdoutLogger;

	res = ht::charsetConverterOpen(stdoutLogger->converter, "UTF-8", "UTF-32LE", &converterCb);
	if (res < 0) {
		goto error;
	}

	stdoutLogger->out.write = writeStdout;
	stdoutLogger->out.userdata = stdoutLogger;

	return;

error:
	if (stdoutLogger != NULL) {
		ht::charsetConverterDestroy(stdoutLogger->converter);
		delete stdoutLogger;
		stdoutLogger = NULL;
	}
}

int logStdout(ht::Log::Priority priority, const char32_t *tag, const char32_t *format, va_list ap)
{
	int res;

	if (stdoutLogger == NULL) {
		initStdout();
	}

	res = fprintf32(&stdoutLogger->out, U"[%s][%s] ", strLogPriority[priority], tag);
	res += vfprintf32(&stdoutLogger->out, format, ap);
	res += fprintf32(&stdoutLogger->out, U"\n");

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

