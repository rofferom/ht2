#ifndef __HTLUA_LUAPARAM_HPP__
#define __HTLUA_LUAPARAM_HPP__

namespace htlua {

template <typename T>
using LuaParamRemoveConstRef = typename std::remove_const<typename std::remove_reference<T>::type>::type;

template <typename T>
using LuaParamClean = typename std::remove_const<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::type;

template <typename T>
using LuaParamIsConst = std::is_const<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>;

// Prototypes
template <typename T, typename... Args>
static bool checkParamList(lua_State *L, int argIndex);

template<int... S, typename... Args>
static int fillParamListInternal(lua_State *L, int id, Sequence<S...>, std::tuple<Args...> &t);

template<typename T, typename... Args>
static int fillVariableList(lua_State *L, int id, T &t, Args (&...args));

static int fillVariableList(lua_State *L, int id);

template <typename T, typename... Args>
static void paramListToString(int id, std::string *out);

// Public API
template <typename... Params>
struct LuaParam {
	static bool check(lua_State *L, int id)
	{
		// First argument is object instance
		return checkParamList<Params...>(L, id);
	}

	static std::string toString()
	{
		std::string out;
		paramListToString<Params..., LuaTypeEnd>(0, &out);
		return out;
	}
};

template <typename... Params>
int luaParamFillList(lua_State *L, int id, std::tuple<Params...> &t)
{
	return fillParamListInternal(L, id, typename SequenceGenerator<sizeof...(Params)>::type(), t);
}

// Prototypes implementation
template <typename T>
bool checkParam(lua_State *L, int argIndex)
{
	using CleanedT = LuaParamClean<T>;
	constexpr bool typeConst = LuaParamIsConst<T>::value;

	static_assert(LuaType<CleanedT>::isValid, "Invalid type");
	return LuaType<CleanedT>::isParamValid(L, argIndex, typeConst);
}

template <typename T, typename... Args>
static bool checkParamList(lua_State *L, int argIndex)
{
	if (checkParam<T>(L, argIndex) == false) {
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

static int fillVariableList(lua_State *L, int id)
{
	return 0;
}

template<typename T, typename... Args>
static int fillVariableList(lua_State *L, int id, T &t, Args (&...args))
{
	using CleanedT = typename std::remove_pointer<T>::type;

	// Here id is the C-array index
	// At this point, const and ref has been removed from T. Is still can be a pointer
	LuaType<CleanedT>::getValue(L, id, t);
	return fillVariableList(L, id + 1, args...);
}

template<int... S, typename... Args>
static int fillParamListInternal(lua_State *L, int id, Sequence<S...>, std::tuple<Args...> &t)
{
	return fillVariableList(L, id, std::get<S>(t)...);
}

template <typename T, typename... Args>
static void paramListToString(int id, std::string *out)
{
	using CleanedT = LuaParamClean<T>;

	static_assert(LuaType<CleanedT>::isValid, "Invalid type");

	if (id == 0) {
		out->append("(");
		out->append(LuaType<CleanedT>::name);
	} else {
		out->append(", ");
		out->append(LuaType<CleanedT>::name);
	}

	paramListToString<Args...>(id + 1, out);
}

template <>
void paramListToString<LuaTypeEnd>(int id, std::string *out)
{
	out->append(")");
}

typedef std::string (*LuaSigGetter)(const char *functionName);

template <typename T>
struct LuaSig;

template <typename R, typename... Args>
struct LuaSig<R(Args...)> {
	static std::string get(const char *functionName)
	{
		std::string out;

		using CleanedR = LuaParamClean<R>;
		static_assert(LuaType<CleanedR>::isValid, "Invalid type");

		out = LuaType<CleanedR>::name;
		out.append(" ");
		out.append(functionName);

		if (sizeof...(Args) > 0) {
			paramListToString<Args..., LuaTypeEnd>(0, &out);
		} else {
			out.append("()");
		}

		return out;
	}
};

} // namespace htlua

#endif // !__HTLUA_LUAPARAM_HPP__
