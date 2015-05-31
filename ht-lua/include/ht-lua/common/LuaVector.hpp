#ifndef __HTLUA_VECTOR_HPP__
#define __HTLUA_VECTOR_HPP__

namespace htlua {

template <typename T>
struct LuaVector;

template <typename T>
struct LuaUserType<htlua::LuaVector<T>> {
	using TVector = std::vector<T>;

	enum { isValid = 1 };

	static bool isParamValid(lua_State *L, int argIndex, bool typeConst)
	{
		return LuaObjectParam<TVector>::isParamValid(L, argIndex, typeConst);
	}

	static void pushValue(lua_State *L, std::vector<T> *t)
	{
		LuaClass<TVector>::forwardReference(L, t);
	}

	static void pushValue(lua_State *L, const std::vector<T> *t)
	{
		LuaClass<TVector>::forwardReference(L, t);
	}

	static void getValue(lua_State *L, int argIndex, std::vector<T> *&t, bool takeOwnership)
	{
		LuaObjectParam<TVector>::getValue(L, argIndex, t, takeOwnership);
	}

	static void getValue(lua_State *L, int argIndex, const std::vector<T> *&t)
	{
		LuaObjectParam<TVector>::getValue(L, argIndex, t);
	}
};

template <typename T>
struct LuaVector : LuaClass<std::vector<T>> {
	using LuaVectorClass = LuaClass<std::vector<T>>;
	using Method = typename LuaVectorClass::Method;

	template <typename U>
	using MethodGenerator = typename LuaVectorClass::template MethodGenerator<U>;

	static void init()
	{
		static Method methods[] = {
			{ "getSize", MethodGenerator<size_t(void)>::get(getSize) },
			{ "pushItem", MethodGenerator<void(T)>::get(pushItem) },
			{ "getItem", MethodGenerator<T(size_t)>::get(getItem) },
			Method::empty(),
		};

		LuaPackage::splitFullName(LuaType<LuaVector<T>>::name, &LuaVectorClass::mPackage,
				&LuaVectorClass::mName);
		LuaVectorClass::mMethods = methods;
	}

	static int getSize(lua_State *L, std::vector<T> *vector)
	{
		LuaType<size_t>::pushValue(L, vector->size());
		return 1;
	}

	static int pushItem(lua_State *L, std::vector<T> *vector)
	{
		T *t;

		htlua::LuaUserType<T>::getValue(L, 2, t, false);
		vector->push_back(*t);

		return 0;
	}

	static int getItem(lua_State *L, std::vector<T> *vector)
	{
		using CleanT = LuaParamClean<T>;

		size_t index;

		LuaType<size_t>::getValue(L, 2, index);
		LuaClass<CleanT>::forwardReference(L, vector->at(index));

		return 1;
	}
};

} // namespace htlua

#endif // !__HTLUA_VECTOR_HPP__
