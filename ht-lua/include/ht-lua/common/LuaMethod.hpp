#ifndef __HTLUA_LUAMETHOD_HPP__
#define __HTLUA_LUAMETHOD_HPP__

namespace htlua {

static const char32_t *LUAMETHOD_TAG = U"LuaMethod";

typedef std::function<int(lua_State *)> LuaMethodHandler;

//template <typename T>
//using LuaMethodHandlerGen = std::function<LuaMethodHandler(T *)>;

template <typename T>
struct LuaMethodHandlerGen {
	std::function<LuaMethodHandler(T *)> mCreate;
	bool mConst;
};

template <typename T>
struct LuaMethodParamChecker;

template <typename R, typename... Args>
struct LuaMethodParamChecker<R(Args...)> {
	static bool check(lua_State *L, int ignoreArgs)
	{
		const int argCount = lua_gettop(L) - ignoreArgs;
		const int expectedArgCount = sizeof...(Args);

		// First argument is object instance
		if (argCount != expectedArgCount) {
			std::string sign = LuaParam<Args...>::toString();

			luaL_error(
				L,
				"Invalid parameter count (got %d, function signature : %s)",
				argCount,
				sign.c_str());

			return false;
		}

		if (expectedArgCount > 0 && !LuaParam<Args..., LuaTypeEnd>::check(L, ignoreArgs + 1)) {
			std::string sign = LuaParam<Args...>::toString();

			luaL_error(
				L,
				"Invalid parameter types (function signature : %s)",
				sign.c_str());

			return false;
		}

		return true;
	}
};

template <typename T>
struct LuaMethodBinder;

template <typename R, typename... Args>
struct LuaMethodBinder<R(Args...)> {
	std::function<R(Args...)> mCb;
	int mIgnoreCount;
	using MethodParamList = std::tuple<LuaParamRemoveConstRef<Args>...>;

	LuaMethodBinder(std::function<R(Args...)> cb, int ignoreCount)
	{
		mCb = cb;
		mIgnoreCount = ignoreCount;
	}

	template<int ...S>
	R callMethod(Sequence<S...>, MethodParamList &args)
	{
		return mCb(std::get<S>(args)...);
	}

	int operator()(lua_State *L)
	{
		const int expectedArgCount = sizeof...(Args);
		MethodParamList args;
		R r;

		if (LuaMethodParamChecker<R(Args...)>::check(L, mIgnoreCount) == false) {
			return 0;
		}

		luaParamFillList(L, mIgnoreCount + 1, args);
		r = callMethod(typename SequenceGenerator<expectedArgCount>::type(), args);
		LuaType<R>::pushValue(L, r);

		return 1;
	}
};

template <typename... Args>
struct LuaMethodBinder<void(Args...)> {
	std::function<void(Args...)> mCb;
	int mIgnoreCount;
	using MethodParamList = std::tuple<LuaParamRemoveConstRef<Args>...>;

	LuaMethodBinder(std::function<void(Args...)> cb, int ignoreCount)
	{
		mCb = cb;
		mIgnoreCount = ignoreCount;
	}

	template<int ...S>
	void callMethod(Sequence<S...>, MethodParamList &args)
	{
		mCb(std::get<S>(args)...);
	}

	int operator()(lua_State *L)
	{
		const int expectedArgCount = sizeof...(Args);
		MethodParamList args;

		if (LuaMethodParamChecker<void(Args...)>::check(L, mIgnoreCount) == false) {
			return 0;
		}

		luaParamFillList(L, mIgnoreCount + 1, args);
		callMethod(typename SequenceGenerator<expectedArgCount>::type(), args);

		return 0;
	}
};

template <typename R>
struct LuaMethodBinder<R(void)> {
	std::function<R(void)> mCb;

	LuaMethodBinder(std::function<R(void)> cb, int ignoreCount)
	{
		mCb = cb;
	}

	int operator()(lua_State *L)
	{
		const int argCount = lua_gettop(L) - 1;
		R r;

		// First argument is object instance
		if (argCount > 0) {
			ht::Log::d(
				LUAMETHOD_TAG,
				U"Invalid parameter count (got %d, 0 expected)",
				argCount);

			return 0;
		}

		r = mCb();
		LuaType<R>::pushValue(L, r);

		return 1;
	}
};

template <>
struct LuaMethodBinder<void()> {
	std::function<void()> mCb;

	LuaMethodBinder(std::function<void()> cb, int ignoreCount)
	{
		mCb = cb;
	}

	int operator()(lua_State *L)
	{
		const int argCount = lua_gettop(L) - 1;

		// First argument is object instance
		if (argCount > 0) {
			ht::Log::d(
				LUAMETHOD_TAG,
				U"Invalid parameter count (got %d, 0 expected)",
				argCount);

			return 0;
		}

		mCb();

		return 0;
	}
};

} // namespace htlua

#endif // !__HTLUA_LUAMETHOD_HPP__
