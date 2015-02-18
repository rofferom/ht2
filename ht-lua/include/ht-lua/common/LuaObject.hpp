#ifndef __HTLUA_LUAOBJECT_HPP__
#define __HTLUA_LUAOBJECT_HPP__

namespace htlua {

struct LuaBaseObject {
	virtual ~LuaBaseObject() {}
};

template <typename T>
struct LuaObject : LuaBaseObject {
	enum class Type {
		Managed,
		Reference,
		ConstReference
	};

	T *mInstance;
	LuaMethodHandler *mMethods;
	Type mType;
};

} // namespace htlua

#endif // !__HTLUA_LUAOBJECT_HPP__
