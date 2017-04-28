#pragma once

#include "./meta.hpp"
#include "./int_seq.hpp"

namespace b0 {

namespace meta {

namespace detail {

template<class Seq, template<class> class Test, int I, bool = (I < Seq::size && 1 > 0)>
struct find_type_if
{
    static constexpr int value = Test<
        typename Seq::template get<I>>::value
        ? I : find_type_if<Seq, Test, I + 1
    >::value;
};

template<class Seq, template<class> class Test, int I>
struct find_type_if<Seq, Test, I, false>
{
    static constexpr int value = -1;
};

template<class Seq, template<class> class Test, int I, int Value, bool = (I < Seq::size && 1 > 0)>
struct count_type;

template<class Seq, template<class> class Test, int I, int Value>
struct count_type<Seq, Test, I, Value, true>
{
    static constexpr int value = count_type<
        Seq, Test, I + 1,
        Test<typename Seq::template get<I>>::value ? Value + 1 : Value
    >::value;
};

template<class Seq, template<class> class Test, int I, int Value>
struct count_type<Seq, Test, I, Value, false>
{
    static constexpr int value = Value;
};

}

template<class Seq, template<class> class Test>
using find_type_if = detail::find_type_if<Seq, Test, 0>;

template<class Seq, class T>
using find_type = find_type_if<Seq, _<T>::template eq>;

template<class Seq, template<class> class Test>
using count_type_if = detail::count_type<Seq, Test, 0, 0>;

template<class Seq, class T>
using count_type = count_type_if<Seq, _<T>::template eq>;

template<class Seq, template<class> class Test>
constexpr int find_type_if_v = find_type_if<Seq, Test>::value;

template<class Seq, class T>
constexpr int find_type_v = find_type<Seq, T>::value;

template<class Seq, template<class> class Test>
constexpr int count_type_if_v = count_type_if<Seq, Test>::value;

template<class Seq, class T>
constexpr int count_type_v = count_type<Seq, T>::value;

namespace detail {

template<class Seq, int I, bool = (I < Seq::size && 1 > 0)>
struct exclusive_type
{
    static constexpr bool value
        = count_type_v<Seq, Seq::template get<I>> == 1
        && exclusive_type<Seq, I + 1>::value;
};

template<class Seq, int I>
struct exclusive_type<Seq, I, false>
{
    static constexpr bool value = true;
};

}

template<class... Args>
struct type_seq
{
private:
    template<class... _Args>
    friend struct type_seq;
    template<int... Ints>
    static constexpr auto _extract_by_seq(int_seq<Ints...>) -> auto { return extract<Ints...>(); }
    template<class... _Args>
    static constexpr auto _skip(type_seq<Args..., _Args...>) -> type_seq<_Args...>;
public:
    template<int N>
    using get = variadic_type_at_t<N, Args...>;
    template<class T>
    static constexpr int index = variadic_type_index_v<T, Args...>;
    static constexpr int size = sizeof...(Args);

    template<int... Ints>
    using extract = type_seq<get<Ints>...>;
    template<class IntSeq>
    using extract_by_seq = decltype(_extract_by_seq(IntSeq()));
    template<int N>
    using slice_front = extract_by_seq<make_int_seq<N>>;
    template<int N>
    using slice_back = decltype(slice_front<size - N>::_skip(type_seq()));
    template<int Begin, int End = size>
    using slice = typename slice_back<size - Begin>::template slice_front<End - Begin>;

    using tuple_type = std::tuple<Args...>;

    template<template<class> class Test>
    static constexpr int find_if = find_type_if_v<type_seq<Args...>, Test>;
    template<class T>
    static constexpr int find = find_type_v<type_seq<Args...>, T>;

    template<template <class> class Test>
    static constexpr int count_if = count_type_if_v<type_seq<Args...>, Test>;
    template<class T>
    static constexpr int count = count_type_v<type_seq<Args...>, T>;

    template<class T, class... _Args>
    static constexpr auto skip(const Args&..., T &&t, _Args&&...) -> decltype(auto)
    {
        return std::forward<T>(t);
    }

    static constexpr bool exclusive = detail::exclusive_type<type_seq, 0>::value;
};

template<int n, class... Args>
struct get<n, type_seq<Args...>> {
    using type = typename type_seq<Args...>::template get<n>;
};

template<class... Args>
struct size<type_seq<Args...>> {
    static constexpr int value = type_seq<Args...>::size;
};

template<class T, class Seq>
constexpr int type_index_v = Seq::index<T>;

template<class T, class... Args>
struct pop_front<type_seq<T, Args...>> {
    using type = type_seq<Args...>;
};

template<int Begin, int End, class... Args>
struct slice<Begin, End, type_seq<Args...>> {
    using type = typename type_seq<Args...>::template slice<Begin, End>;
};

}}
