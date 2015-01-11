#ifndef __HTLUA_LUAPARAM_HPP__
#define __HTLUA_LUAPARAM_HPP__

#include <string>
#include <tuple>
#include <ht-lua/common/LuaClass.hpp>
#include <ht-lua/common/LuaTypes.hpp>
#include <ht-lua/common/Sequence.hpp>

namespace htlua {

// Prototypes
template <typename T, typename... Args>
bool checkParamList(lua_State *L, int argIndex);

template<int... S, typename... Args>
int fillParamList(lua_State *L, int id, Sequence<S...>, std::tuple<Args...> &t);

template<typename T, typename... Args>
int fillVariableList(lua_State *L, int id, T &t, Args (&...args));

int fillVariableList(lua_State *L, int id);

template <typename T, typename... Args>
void paramListToString(int id, std::string *out);

// Public API
template <typename ...Params>
struct LuaParam {
	static bool check(lua_State *L, int id)
	{
		// First argument is object instance
		return checkParamList<Params...>(L, id);
	}

	static int fill(lua_State *L, int id, std::tuple<Params...> &t)
	{
		return fillParamList(L, id, typename SequenceGenerator<sizeof...(Params)>::type(), t);
	}

	static std::string toString()
	{
		std::string out;
		paramListToString<Params..., LuaTypeEnd>(0, &out);
		return out;
	}
};

// Prototypes implementation
template <typename T, typename... Args>
bool checkParamList(lua_State *L, int argIndex)
{
	static_assert(LuaType<T>::known, "Invalid type");

	if (LuaType<T>::isParamValid(L, argIndex) == false) {
		return false;
	} else {
		return checkParamList<Args...>(L, argIndex + 1);
	}
}

template <>
bool checkParamList<void>(lua_State *L, int argIndex)
{
	return true;
}

template <>
bool checkParamList<LuaTypeEnd>(lua_State *L, int argIndex)
{
	return true;
}

int fillVariableList(lua_State *L, int id)
{
	return 0;
}

template<typename T, typename... Args>
int fillVariableList(lua_State *L, int id, T &t, Args (&...args))
{
	// Here id is the C-array index
	LuaType<T>::getValue(L, id, t);
	return fillVariableList(L, id + 1, args...);
}

template<int... S, typename... Args>
int fillParamList(lua_State *L, int id, Sequence<S...>, std::tuple<Args...> &t)
{
	return fillVariableList(L, id, std::get<S>(t)...);
}

template <typename T, typename... Args>
void paramListToString(int id, std::string *out)
{
	if (id == 0) {
		out->append("(");
		out->append(LuaType<T>::name);
	} else {
		out->append(", ");
		out->append(LuaType<T>::name);
	}

	paramListToString<Args...>(id + 1, out);
}

template <>
void paramListToString<LuaTypeEnd>(int id, std::string *out)
{
	out->append(")");
}

} // namespace htlua

#endif // !__HTLUA_LUAPARAM_HPP__
