#ifndef __HTLUA_LUAFUNCTION_HPP__
#define __HTLUA_LUAFUNCTION_HPP__

namespace htlua {

template <typename T>
struct LuaFunction {
	// Class description part
	struct FunctionHandler {
		LuaMethodHandler mCb;
		LuaSigGetter mSig;
	};

	struct Function {
		const char *mName;
		FunctionHandler mHandler;

		static Function empty()
		{
			return { nullptr, { nullptr, nullptr } };
		}
	};

	static const char *mPackage;
	static Function *mFunctions;

	// FunctionGenerator
	template <typename U>
	struct FunctionGenerator;

	template <typename R, typename... Args>
	struct FunctionGenerator<R(Args...)> {
		static FunctionHandler get(R (*function)(Args...))
		{
			auto luaCb = [function] (lua_State *L) mutable -> int {
				std::function<R(Args...)> cb = function;
				LuaMethodBinder<R(Args...)> binder(cb, 0);
				return binder(L);
			};

			return FunctionHandler { luaCb, LuaSig<R(Args...)>::get };
		}

		static FunctionHandler get(int (*cb)(lua_State *L))
		{
			auto luaCb = [cb] (lua_State *L) mutable -> int {
				if (LuaMethodParamChecker<R(Args...)>::check(L, 0) == false) {
					return 0;
				}

				return cb(L);
			};

			return FunctionHandler { luaCb, LuaSig<R(Args...)>::get };
		}
	};

	static int functionHandler(lua_State *L)
	{
		int functionId;

		functionId = (int) lua_tonumber(L, lua_upvalueindex(1));

		return mFunctions[functionId].mHandler.mCb(L);
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

	static void printFunction()
	{
		printf("Functions :\n");

		for (const Function *i = mFunctions ; i->mName != nullptr ; i++) {
			if (mPackage != nullptr) {
				std::string name;

				name = mPackage;
				name.append(".");
				name.append(i->mName);

				printf("\t%s\n", i->mHandler.mSig(name.c_str()).c_str());
			} else {
				printf("\t%s\n", i->mHandler.mSig(i->mName).c_str());
			}
		}
	}
};

template <typename T>
const char *LuaFunction<T>::mPackage = NULL;

template <typename T>
typename LuaFunction<T>::Function *LuaFunction<T>::mFunctions = NULL;

} // namespace htlua

#endif // !__HTLUA_LUAFUNCTION_HPP__
