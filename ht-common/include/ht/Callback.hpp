#ifndef __HT_CALLBACK_HPP
#define __HT_CALLBACK_HPP

#include <assert.h>

namespace ht {

template <typename T>
class Callback;

template <typename R, typename... Args>
class Callback<R(Args...)> {
private:
	class Binder {};

private:
	Binder *mInstance;
	R (Binder::*mMethod)(Args... args);

public:
	Callback()
	{
		mInstance = nullptr;
		mMethod = nullptr;
	}

	template <typename T>
	Callback(T *t, R (T::*c)(Args...))
	{
		mInstance = reinterpret_cast<Binder *>(t);
		mMethod = reinterpret_cast<R (Binder:: *)(Args... args)>(c);
	}

	template <typename T>
	Callback(T *t, R (T::*c)(Args...) const)
	{
		mInstance = reinterpret_cast<Binder *>(t);
		mMethod = reinterpret_cast<R (Binder:: *)(Args... args)>(c);
	}

	R operator()(Args... args) const
	{
		assert(mInstance != NULL);
		assert(mMethod != NULL);
		return (mInstance->*mMethod)(args...);
	}

	operator bool() const
	{
		return (mInstance != NULL) && (mMethod != NULL);
	}
};

} // namespace ht

#endif // !__HT_CALLBACK_HPP
