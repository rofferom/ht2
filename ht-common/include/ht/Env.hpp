#ifndef __HT_ENV_HPP
#define __HT_ENV_HPP

namespace ht {

struct InternalCharset {
	constexpr static const char *name = "UTF-32LE";
};

class SysEnv {
public:
	HTAPI static int setCharset(const char *charset);

	HTAPI static int toInternal(
		const void *rawContent,
		size_t rawContentSize,
		const CharsetConverter::Cb &cb);

	HTAPI static std::u32string toInternal(const std::string &s);

	HTAPI static int toSysEnv(const void *rawContent,
		size_t rawContentSize,
		const CharsetConverter::Cb &cb);

	HTAPI static std::string toSysEnv(const std::u32string &s);
};

} // ht

#endif // __HT_ENV_HPP
