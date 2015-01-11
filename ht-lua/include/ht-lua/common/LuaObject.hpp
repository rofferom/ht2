#ifndef __HTLUA_LUAOBJECT_HPP__
#define __HTLUA_LUAOBJECT_HPP__

#include <ht-lua/common/LuaMethod.hpp>

namespace htlua {

template <typename T>
struct LuaObject {
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
