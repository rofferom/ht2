#ifndef __HT_LUA_HPP__
#define __HT_LUA_HPP__

#include <assert.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include <ht.hpp>

#include <ht-lua/common/Sequence.hpp>
#include <ht-lua/common/LuaTypes.hpp>
#include <ht-lua/common/LuaParam.hpp>
#include <ht-lua/common/LuaMethod.hpp>
#include <ht-lua/common/LuaObject.hpp>
#include <ht-lua/common/LuaPackage.hpp>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaEnum.hpp>
#include <ht-lua/common/LuaCallback.hpp>
#include <ht-lua/common/LuaFunction.hpp>
#include <ht-lua/common/LuaObjectParam.hpp>
#include <ht-lua/common/LuaUserType.hpp>
#include <ht-lua/common/LuaUserEnum.hpp>
#include <ht-lua/common/LuaVector.hpp>

#include <ht-lua/LuaBuffer.hpp>
#include <ht-lua/LuaFile.hpp>
#include <ht-lua/LuaPointer.hpp>
#include <ht-lua/LuaTable.hpp>
#include <ht-lua/LuaText.hpp>

namespace htlua {

HTAPI int registerComponents(lua_State *L);
HTAPI void printComponents();

} // namespace htlua

#endif // !__HT_LUA_HPP__
