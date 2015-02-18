#include "ht-private.hpp"

namespace {

struct SysEnvInternal {
	~SysEnvInternal();
	int configure();
	void clear();

	std::string mCharset = "UTF-8";

	ht::CharsetConverter *mToInternal = NULL;
	ht::CharsetConverter *mToSysEnv = NULL;
};

SysEnvInternal::~SysEnvInternal()
{
	clear();
}

int SysEnvInternal::configure()
{
	int res;

	// Configure mToInternal
	mToInternal = ht::CharsetConverter::create();
	if (mToInternal == NULL) {
		res = -ENOMEM;
		goto error;
	}

	res = mToInternal->open(ht::InternalCharset::name, mCharset.c_str());
	if (res < 0) {
		goto error;
	}

	// Configure mToSysEnv
	mToSysEnv = ht::CharsetConverter::create();
	if (mToSysEnv == NULL) {
		res = -ENOMEM;
		goto error;
	}

	res = mToSysEnv->open(mCharset.c_str(), ht::InternalCharset::name);
	if (res < 0) {
		goto error;
	}

	return 0;

error:
	clear();

	return res;
}

void SysEnvInternal::clear()
{
	if (mToInternal != NULL) {
		mToInternal->close();
		ht::CharsetConverter::destroy(mToInternal);
		mToInternal = NULL;
	}

	if (mToSysEnv != NULL) {
		mToSysEnv->close();
		ht::CharsetConverter::destroy(mToSysEnv);
		mToSysEnv = NULL;
	}
}

SysEnvInternal sysEnv;

} // anonymous namespace

namespace ht {

int SysEnv::setCharset(const char *charset)
{
	sysEnv.clear();
	sysEnv.mCharset = charset;

	return 0;
}

int SysEnv::toInternal(
	const void *rawContent,
	size_t rawContentSize,
	const CharsetConverter::Cb &cb)
{
	if (sysEnv.mToInternal == NULL) {
		int res = sysEnv.configure();
		if (res < 0) {
			return 0;
		}
	}

	return sysEnv.mToInternal->input(rawContent, rawContentSize, cb);
}

std::u32string SysEnv::toInternal(const std::string &s)
{
	std::u32string out;
	CharsetConverter::Cb cb;

	cb.output = [&out] (const void *buff, size_t size) -> int {
		out.append((const char32_t *) buff, size / sizeof(char32_t));
		return 0;
	};

	cb.invalidSeq = [&out] () -> int {
		out.append(U"?");
		return 0;
	};

	if (sysEnv.mToInternal == NULL) {
		sysEnv.configure();
	}

	sysEnv.mToInternal->input(s.c_str(), s.size(), cb);

	return out;
}

int SysEnv::toSysEnv(
	const void *rawContent,
	size_t rawContentSize,
	const CharsetConverter::Cb &cb)
{
	if (sysEnv.mToSysEnv == NULL) {
		int res = sysEnv.configure();
		if (res < 0) {
			return 0;
		}
	}

	return sysEnv.mToSysEnv->input(rawContent, rawContentSize, cb);
}

std::string SysEnv::toSysEnv(const std::u32string &s)
{
	std::string out;
	CharsetConverter::Cb cb;

	cb.output = [&out] (const void *buff, size_t size) -> int {
		out.append((const char *) buff, size);
		return 0;
	};

	cb.invalidSeq = [&out] () -> int {
		out.append("?");
		return 0;
	};

	if (sysEnv.mToSysEnv == NULL) {
		sysEnv.configure();
	}

	sysEnv.mToSysEnv->input(s.c_str(), s.size() * sizeof(char32_t), cb);

	return out;
}

} // ht
