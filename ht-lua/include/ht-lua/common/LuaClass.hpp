#ifndef __HTLUA_LUACLASS_HPP__
#define __HTLUA_LUACLASS_HPP__

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include <assert.h>
#include <cstddef>
#include <functional>
#include <ht/Log.hpp>
#include <ht/Utils.hpp>
#include <ht-lua/common/LuaObject.hpp>
#include <ht-lua/common/LuaTypes.hpp>
#include <ht-lua/common/LuaParam.hpp>
#include <ht-lua/common/LuaPackage.hpp>

namespace htlua {

template <typename T>
struct LuaClass {
	// Class description part
	struct Method {
		const char *mName;
		LuaMethodHandlerGen<T> mHandlerGen;

		static Method empty()
		{
			return { nullptr, { nullptr, false } };
		}
	};

	static const char *mName;
	static const char *mPackage;
	static Method *mMethods;
	static size_t mMethodCount;
	static const char32_t *TAG;

	// MethodGenerator
	template <typename U>
	struct MethodGenerator;

	template <typename R, typename... Args>
	struct MethodGenerator<R(Args...)> {
		static LuaMethodHandlerGen<T> get(R (T::*method)(Args...)) {
			auto createCb = [method] (T *t) -> LuaMethodHandler {
				ht::Callback<R(Args...)> cb(t, method);

				return [cb] (lua_State *L) mutable -> int {
					LuaMethodBinder<R(Args...)> binder(cb);
					return binder(L);
				};
			};

			return LuaMethodHandlerGen<T> { createCb, false };
		}

		static LuaMethodHandlerGen<T> get(R (T::*method)(Args...) const) {
			auto createCb = [method] (T *t) -> LuaMethodHandler {
				ht::Callback<R(Args...)> cb(t, method);

				return [cb] (lua_State *L) -> int {
					LuaMethodBinder<R(Args...)> binder(cb);
					return binder(L);
				};
			};

			return LuaMethodHandlerGen<T> { createCb, true };
		}

		static LuaMethodHandlerGen<T> get(int (*cb)(lua_State *L, T *t), bool isConst = false) {
			auto createCb = [cb] (T *t) -> LuaMethodHandler {
				return [cb, t] (lua_State *L) mutable -> int {
					if (LuaMethodParamChecker<R(Args...)>::check(L) == false) {
						return 0;
					}

					return cb(L, t);
				};
			};

			return LuaMethodHandlerGen<T> { createCb, isConst };
		}
	};

	template <typename U>
	struct GetSetGenerator {
		static LuaMethodHandlerGen<T> get(size_t offset)
		{
			auto createCb = [offset] (T *t) -> LuaMethodHandler {
				return [offset, t] (lua_State *L) mutable -> int {
					U *u;

					if (LuaMethodParamChecker<U(void)>::check(L) == false) {
						return 0;
					}

					u = (U *) ((char *) t + offset);
					LuaType<U>::pushValue(L, *u);
					return 1;
				};
			};

			return LuaMethodHandlerGen<T> { createCb, true };
		}

		static LuaMethodHandlerGen<T> set(size_t offset)
		{
			auto createCb = [offset] (T *t) -> LuaMethodHandler {
				return [offset, t] (lua_State *L) mutable -> int {
					U *u;

					if (LuaMethodParamChecker<void(U)>::check(L) == false) {
						return 0;
					}

					u = (U *) ((char *) t + offset);
					LuaType<U>::getValue(L, 2, *u);

					return 0;
				};
			};

			return LuaMethodHandlerGen<T> { createCb, false };
		}
	};

	// Class registration part
	static int methodHandler(lua_State *L)
	{
		LuaObject<T> *self;
		int methodId;

		self = (LuaObject<T> *) lua_touserdata(L, 1);
		methodId = (int) lua_tonumber(L, lua_upvalueindex(1));

		ht::Log::d(TAG, U"Calling method %d on object %p", methodId, self);
		return self->mMethods[methodId](L);
	}

	static int constructorHandler(lua_State *L)
	{
		LuaObject<T> *instance;

		// Create userdata
		instance = (LuaObject<T> *) lua_newuserdata(L, sizeof(LuaObject<T>));
		new(instance) LuaObject<T>();

		// Force initialization the object allocated by Lua
		instance->mInstance = new T();
		instance->mType = LuaObject<T>::Type::Managed;

		buildMethods(L, instance);

		return 1;
	}

	static int forwardReference(lua_State *L, T *t)
	{
		LuaObject<T> *instance;

		// Create userdata
		instance = (LuaObject<T> *) lua_newuserdata(L, sizeof(LuaObject<T>));
		new(instance) LuaObject<T>();

		// Force initialization the object allocated by Lua
		instance->mInstance = t;
		instance->mType = LuaObject<T>::Type::Reference;

		buildMethods(L, instance);

		return 1;
	}

	static int forwardReference(lua_State *L, const T *t)
	{
		LuaObject<T> *instance;

		// Create userdata
		instance = (LuaObject<T> *) lua_newuserdata(L, sizeof(LuaObject<T>));
		new(instance) LuaObject<T>();

		// Force initialization the object allocated by Lua
		instance->mInstance = const_cast<T *>(t);
		instance->mType = LuaObject<T>::Type::ConstReference;

		buildMethods(L, instance);

		return 1;
	}

	static int invalidConstObjectCall(lua_State *L)
	{
		luaL_error(L, "Trying to call a non const method on a const object");
		return 0;
	}

	static int buildMethods(lua_State *L, LuaObject<T> *instance)
	{
		// Link methods
		ht::Log::d(TAG, U"Linking %d methods on object %p", mMethodCount, instance);
		instance->mMethods = new LuaMethodHandler[mMethodCount];

		for (size_t i = 0 ; i < mMethodCount ; i++) {
			ht::Log::d(TAG, U"Method %d is const : %d", i, mMethods[i].mHandlerGen.mConst);

			if (instance->mType != LuaObject<T>::Type::ConstReference) {
				ht::Log::d(TAG, U"Object not const, normal link");
				instance->mMethods[i] = mMethods[i].mHandlerGen.mCreate(instance->mInstance);
			} else if (mMethods[i].mHandlerGen.mConst == true) {
				ht::Log::d(TAG, U"Object const, const method : link");
				instance->mMethods[i] = mMethods[i].mHandlerGen.mCreate(instance->mInstance);
			} else {
				ht::Log::d(TAG, U"Object const, not const method : dont link");
				instance->mMethods[i] = invalidConstObjectCall;
			}
		}

		// Create metatable
		createMetatable(L);

		return 0;
	}

	static int destructorHandler(lua_State *L)
	{
		LuaObject<T> *instance = (LuaObject<T> *) lua_touserdata(L, 1);

		if (instance->mType == LuaObject<T>::Type::Managed) {
			delete instance->mInstance;
		}

		delete[] instance->mMethods;
		instance->~LuaObject<T>();

		return 0;
	}

	static int createMetatable(lua_State *L)
	{
		// Create metatable
		lua_newtable(L);

		// Metatable.__index = Metatable
		lua_pushstring(L, "__index");
		lua_pushvalue(L, -2); // Push the metatable
		lua_settable(L, -3);

		// Link to garbage collector
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, &destructorHandler);
		lua_settable(L, -3);

		// Register methods
		ht::Log::d(TAG, U"Registering %d methods", mMethodCount);

		for (size_t i = 0 ; i < mMethodCount ; i++) {
			lua_pushstring(L, mMethods[i].mName);
			lua_pushnumber(L, i); // let the thunk know which method we have called
			lua_pushcclosure(L, &methodHandler, 1);
			lua_settable(L, -3); // self["function"] = thunk("function")
		}

		// Link metatable
		lua_setmetatable(L, -2);

		return 0;
	}

	static int registerClass(lua_State *L)
	{
		for (const Method *i = mMethods ; i->mName != nullptr ; i++) {
			mMethodCount++;
		}

		// Register constructor
		lua_pushnumber(L, 0);
		lua_pushcclosure(L, &constructorHandler, 1);

		if (mPackage != nullptr) {
			LuaPackage::set(L, mPackage, mName);
		} else {
			lua_setglobal(L, mName);
		}

		return 0;
	}
};

template <typename T>
const char *LuaClass<T>::mName = NULL;

template <typename T>
const char *LuaClass<T>::mPackage = NULL;

template <typename T>
typename LuaClass<T>::Method *LuaClass<T>::mMethods = NULL;

template <typename T>
size_t LuaClass<T>::mMethodCount = 0;

template <typename T>
const char32_t *LuaClass<T>::TAG = U"LuaClass";

} // namespace htlua

#endif // !__HTLUA_LUACLASS_HPP__
