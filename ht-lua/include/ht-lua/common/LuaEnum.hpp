#ifndef __HTLUA_LUAENUM_HPP__
#define __HTLUA_LUAENUM_HPP__

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

	static std::string mName;
	static std::string mPackage;
	static Enum *mEnums;

	static int registerEnum(lua_State *L)
	{
		assert(mName.empty() == false);
		assert(mEnums != NULL);

		// Create enum table
		lua_newtable(L);

		// Assign enum table to env.
		// Make sure that after that, -1 index will still be the table
		lua_pushvalue(L, -1);
		if (mPackage.empty() == false) {
			LuaPackage::set(L, mPackage.c_str(), mName.c_str());
		} else {
			lua_setglobal(L, mName.c_str());
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

	static void printEnum()
	{
		if (mPackage.empty() == false) {
			printf("Enum %s.%s :\n", mPackage.c_str(), mName.c_str());
		} else {
			printf("Enum %s :\n", mName.c_str());
		}

		for (const Enum *i = mEnums ; i->mName != nullptr ; i++) {
			printf("\t%s\n", i->mName);
		}
	}
};

template <typename T>
std::string LuaEnum<T>::mPackage;

template <typename T>
std::string LuaEnum<T>::mName;

template <typename T>
typename LuaEnum<T>::Enum *LuaEnum<T>::mEnums = NULL;

} // namespace htlua

#endif // !__HTLUA_LUAENUM_HPP__
