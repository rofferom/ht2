#ifndef __HTLUA_LUAENUM_HPP__
#define __HTLUA_LUAENUM_HPP__

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include <assert.h>
#include <ht-lua/common/LuaPackage.hpp>

namespace htlua {

template <typename T>
struct LuaEnum {
	struct Enum {
		const char *mName;
		T mValue;

		static Enum empty()
		{
			return { nullptr, (T) 0 };
		}
	};

	static const char *mName;
	static const char *mPackage;
	static Enum *mEnums;

	static int registerEnum(lua_State *L)
	{
		assert(mName != NULL);
		assert(mEnums != NULL);

		// Create enum table
		lua_newtable(L);

		// Assign enum table to env.
		// Make sure that after that, -1 index will still be the table
		lua_pushvalue(L, -1);
		if (mPackage != nullptr) {
			LuaPackage::set(L, mPackage, mName);
		} else {
			lua_setglobal(L, mName);
		}

		for (int i = 0 ; mEnums[i].mName != nullptr ; i++) {
			lua_pushstring(L, mEnums[i].mName);
			lua_pushinteger(L, (lua_Integer) mEnums[i].mValue);

			lua_settable(L, -3);
		}

		// Drop the enum table
		lua_pop(L, 1);

		return 0;
	}
};

template <typename T>
const char *LuaEnum<T>::mPackage = NULL;

template <typename T>
const char *LuaEnum<T>::mName = NULL;

template <typename T>
typename LuaEnum<T>::Enum *LuaEnum<T>::mEnums = NULL;

} // namespace htlua

#endif // !__HTLUA_LUAENUM_HPP__
