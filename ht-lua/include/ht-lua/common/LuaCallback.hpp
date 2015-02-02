#ifndef __HTLUA_LUACALLBACK_HPP__
#define __HTLUA_LUACALLBACK_HPP__

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include <assert.h>
#include <functional>
#include <ht-lua/common/LuaTypes.hpp>

namespace htlua {

template <typename R, typename... Args>
struct LuaCallback<R(Args...)> {
	template <typename T>
	static void pushParameters(lua_State *L, T t)
	{
		LuaType<T>::pushValue(L, t);
	}

	template <typename T, typename... Params>
	static void pushParameters(lua_State *L, T t, Params... params)
	{
		LuaType<T>::pushValue(L, t);
		pushParameters(L, params...);
	}

	static std::function<R(Args...)> get(lua_State *L, int index)
	{
		return [L, index] (Args... args) -> R {
			R res;
			int luaRes;

			lua_pushvalue(L, index);
			pushParameters(L, args...);

			luaRes = lua_pcall(L, sizeof...(Args), 1, 0);
			assert(luaRes == LUA_OK);

			LuaType<R>::getValue(L, -1, res);
			lua_pop(L, 1);

			return res;
		};
	}
};

} // namespace htlua

#endif // !__HTLUA_LUACALLBACK_HPP__
