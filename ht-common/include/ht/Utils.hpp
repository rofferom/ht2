#ifndef __HT_UTILS_HPP__
#define __HT_UTILS_HPP__

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <functional>

#define HT_SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))

namespace ht {

template <typename T>
struct Callback;

template <typename R, typename... Args>
struct Callback<R(Args...)> {
	typedef std::function<R(Args...)> Type;

	template <typename T>
	static Type get(T *t, R (T::*method)(Args...))
	{
		return [t, method] (Args... args) mutable -> R {
			return (t->*method)(args...);
		};
	}

	template <typename T>
	static Type get(T *t, R (T::*method)(Args...) const)
	{
		return [t, method] (Args... args) mutable -> R {
			return (t->*method)(args...);
		};
	}

	template <typename T>
	static Type get(R (*cb)(Args...))
	{
		std::function<R(Args...)> res = cb;

		return res;
	}
};

int loadFile(const char *path, char **outContent, size_t *outContentSize);
int dumpToFile(const char *path, const uint8_t *buffer, size_t size);
int compareFiles(const char *path1, const char *path2);

std::u32string convertFromEnvStr(const std::string &s);
std::string convertToEnvStr(const std::u32string &s);

} // ht


#endif // !__HT_UTILS_HPP__
