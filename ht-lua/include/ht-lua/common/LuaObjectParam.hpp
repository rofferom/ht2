#ifndef __HTLUA_LUAOBJECTPARAM_HPP__
#define __HTLUA_LUAOBJECTPARAM_HPP__

namespace htlua {

template <typename T>
struct LuaObjectParam {
	static bool isParamValid(lua_State *L, int argIndex, bool isConst)
	{
		LuaBaseObject *base;
		LuaObject<T> *self;

		if (lua_isuserdata(L, argIndex) == 0) {
			return false;
		}

		base = (LuaBaseObject *) lua_touserdata(L, argIndex);
		if (base == NULL) {
			return false;
		}

		self = dynamic_cast<LuaObject<T> *>(base);
		if (self == NULL) {
			return false;
		}

		if (self->mType == LuaObject<T>::Type::ConstReference && isConst == false) {
			return false;
		}

		return true;
	}

	static void getValue(lua_State *L, int argIndex, T *&t, bool takeOwnership = true)
	{
		LuaBaseObject *base;
		LuaObject<T> *self;

		t = NULL;
		if (lua_isuserdata(L, argIndex) == 0) {
			return;
		}

		base = (LuaBaseObject *) lua_touserdata(L, argIndex);
		if (base == NULL) {
			return;
		}

		self = dynamic_cast<LuaObject<T> *>(base);
		if (self == NULL) {
			return;
		}

		// luaObjectIsParamValid() should have been called before.
		assert(self->mType != LuaObject<T>::Type::ConstReference);
		t = self->mInstance;

		if (takeOwnership == true) {
			assert(self->mType == LuaObject<T>::Type::Managed);
			self->mType = LuaObject<T>::Type::Reference;
		}
	}

	static void getValue(lua_State *L, int argIndex, const T *&t)
	{
		LuaBaseObject *base;
		LuaObject<T> *self;

		t = NULL;
		if (lua_isuserdata(L, argIndex) == 0) {
			return;
		}

		base = (LuaBaseObject *) lua_touserdata(L, argIndex);
		if (base == NULL) {
			return;
		}

		self = dynamic_cast<LuaObject<T> *>(base);
		if (self == NULL) {
			return;
		}

		t = self->mInstance;
	}
};

} // namespace htlua

#endif // !__HTLUA_LUAOBJECTPARAM_HPP__
