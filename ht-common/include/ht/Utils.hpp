#ifndef __HT_UTILS_HPP__
#define __HT_UTILS_HPP__

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

HTAPI int loadFile(const char *path, char **outContent, size_t *outContentSize);
HTAPI int dumpToFile(const char *path, const uint8_t *buffer, size_t size);
HTAPI int compareFiles(const char *path1, const char *path2);

} // ht

#endif // !__HT_UTILS_HPP__
