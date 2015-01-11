#ifndef __HTLUA_SEQUENCE_HPP__
#define __HTLUA_SEQUENCE_HPP__

template<int...> struct Sequence {};

template<int N, int... S> struct SequenceGenerator : SequenceGenerator<N-1, N-1, S...> {};

template<int... S> struct SequenceGenerator<0, S...>{ typedef Sequence<S...> type; };

#endif // !__HTLUA_SEQUENCE_HPP__
