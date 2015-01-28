#ifndef __HTLUA_LUAFUNCTION_HPP__
#define __HTLUA_LUAFUNCTION_HPP__

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include <assert.h>
#include <ht-lua/common/LuaMethod.hpp>
#include <ht-lua/common/LuaPackage.hpp>

namespace htlua {

template <typename T>
struct LuaFunction {
	// Class description part
	struct Function {
		const char *mName;
		LuaMethodHandler mHandler;

		static Function empty()
		{
			return { nullptr, nullptr };
		}
	};

	static const char *mPackage;
	static Function *mFunctions;

	// FunctionGenerator
	template <typename U>
	struct FunctionGenerator;

	template <typename R, typename... Args>
	struct FunctionGenerator<R(Args...)> {
		static LuaMethodHandler get(R (*function)(Args...)) {
				return [function] (lua_State *L) mutable -> int {
					std::function<R(Args...)> cb = function;
					LuaMethodBinder<R(Args...)> binder(cb, 0);
					return binder(L);
			};
		}

		static LuaMethodHandler get(int (*cb)(lua_State *L)) {
				return [cb] (lua_State *L) mutable -> int {
					if (LuaMethodParamChecker<R(Args...)>::check(L, 0) == false) {
						return 0;
					}

					return cb(L);
			};
		}
	};

	static int functionHandler(lua_State *L)
	{
		int functionId;

		functionId = (int) lua_tonumber(L, lua_upvalueindex(1));

		return mFunctions[functionId].mHandler(L);
	}

	static int registerFunction(lua_State *L)
	{
		assert(mFunctions != NULL);

		for (int i = 0 ; mFunctions[i].mName != NULL ; i++) {
			lua_pushnumber(L, i);
			lua_pushcclosure(L, &functionHandler, 1);

			if (mPackage != nullptr) {
				LuaPackage::set(L, mPackage, mFunctions[i].mName);
			} else {
				lua_setglobal(L, mFunctions[i].mName);
			}
		}

		return 0;
	}
};

template <typename T>
const char *LuaFunction<T>::mPackage = NULL;

template <typename T>
typename LuaFunction<T>::Function *LuaFunction<T>::mFunctions = NULL;

} // namespace htlua

#endif // !__HTLUA_LUAFUNCTION_HPP__
