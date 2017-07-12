#pragma once

#include "../global.hpp"
#include <tuple>

#define META_ASSERT(...) \
    static_assert(__VA_ARGS__, "falsy expr: " #__VA_ARGS__)

#define B0_REQ(...) class = typename std::enable_if<__VA_ARGS__>::type
#define B0_REQ_OP(...) b0::meta::req_t<__VA_ARGS__, b0::meta::__b0_req_t> = b0::meta::__b0_req_t_value // optional parameter
#define B0_USING_REQ_T template<bool cond, class T = void> using req_t = b0::meta::req_t<cond, T>
#define B0_USING_EQ_V template<class T, class V> static constexpr bool eq_v = b0::meta::eq_v<T, V>

#define B0_DECLARE_TRAIT(name, ...) \
    template<class __T> class name { \
        template<class T> static auto test(int) -> decltype(__VA_ARGS__, std::true_type{}); \
        template<class T> static auto test(...) -> std::false_type; \
    public: \
        static constexpr bool value = decltype(test<__T>(0))::value; };

#define B0_DECLARE_HAS_MEMBER_WITH_NAME(name, mem) \
    template<class T, bool = std::is_class<T>::value> \
    class name { \
        struct member { int mem; }; \
        struct type : T, member {}; \
        template<class S, S> struct check; \
        template<class S> static auto test(check<int member::*, &S::mem>*) -> std::false_type; \
        template<class S> static auto test(...) -> std::true_type; \
    public: \
        static bool const value = decltype(test<type>(0))::value; }; \
    template<class T> class name<T, false> : public std::false_type {};

#define B0_DECLARE_HAS_MEMBER(mem) B0_DECLARE_HAS_MEMBER_WITH_NAME(has_ ## mem, mem)

#define B0_NOEXCEPT_IF(expr) noexcept(noexcept(expr))
#define B0_NOEXCEPT_RETURN(expr, ret) B0_NOEXCEPT_IF(expr) -> ret
#define B0_NOEXCEPT_RETURN_DECLTYPE(expr) noexcept(noexcept(expr)) -> decltype(expr)
#define B0_RETURN_BODY(expr) B0_NOEXCEPT_RETURN_DECLTYPE(expr) { return expr; }

namespace b0 { namespace meta {

template<bool B>
using bool_ = std::integral_constant<bool, B>;
using true_ = bool_<true>;
using false_ = bool_<false>;

enum __b0_req_t { __b0_req_t_value };

template<class T>
class print_type_t;

template<class T>
auto print_type(T&&) -> void;

struct valid_expr_t {
    constexpr valid_expr_t() = default;
    template<class... Args>
    constexpr auto operator () (Args&&...) const -> void;
};

constexpr valid_expr_t valid_expr{};
#define b0_valid_expr(...) decltype(b0::meta::valid_expr(((__VA_ARGS__), 0)))

struct void_t { };

template<bool cond, class T1, class T2>
using if_t = std::conditional_t<cond, T1, T2>;

template<bool cond, class T = void>
using req_t = std::enable_if_t<cond, T>;

template <class T>
using unconst_t = std::remove_const_t<T>;

template <class T>
using unref_t = std::remove_reference_t<T>;

template <class T>
using uncref_t = unconst_t<unref_t<T>>;

template<class T>
using is_const = std::is_const<unref_t<T>>;

template<class T>
constexpr bool is_const_v = is_const<T>::value;

template<class T>
using is_rref = std::is_rvalue_reference<T>;

template<class T>
using is_lref = std::is_lvalue_reference<T>;

template<class T>
using is_ref = std::is_reference<T>;

template<class T>
constexpr bool is_rref_v = is_rref<T>::value;

template<class T>
constexpr bool is_lref_v = is_lref<T>::value;

template<class T>
constexpr bool is_ref_v = is_ref<T>::value;

template <class T>
struct is_ref_wrapper : false_ {};
template <class U>
struct is_ref_wrapper<std::reference_wrapper<U>> : true_ {};

template <class T>
constexpr bool is_ref_wrapper_v = is_ref_wrapper<T>::value;

template<class T, bool add = true>
using const_t = if_t<add, const T, T>;

template<class T, bool add = true>
using rref_t = if_t<add, T&&, T>;

template<class T, class S>
using eq = std::is_same<T, S>;

template<class T, class S>
static constexpr bool eq_v = eq<T, S>::value;

template<class T>
struct wrap_type { using type = T; };

template<int I>
using int_ = std::integral_constant<int, I>;

template<class T>
struct wrap_void : meta::wrap_type<T> {};

template<>
struct wrap_void<void> : meta::wrap_type<void_t> {};

template<class T>
using wrap_void_t = typename wrap_void<T>::type;

namespace detail {
template<int n, class... Args>
struct variadic_type_at {};

template<int n, class T, class... Args>
struct variadic_type_at<n, T, Args...>
        : wrap_type<typename variadic_type_at<n - 1, Args...>::type> {};

template<class T, class... Args>
struct variadic_type_at<0, T, Args...> : wrap_type<T> {};

}

template<int n, class... Args>
using variadic_type_at = detail::variadic_type_at<n, Args...>;

template<int n, class... Args>
using variadic_type_at_t = typename variadic_type_at<n, Args...>::type;

namespace detail {
template<int offset, class T, class S, class... Args>
struct variadic_type_index {
    static constexpr int value = variadic_type_index<offset + 1, T, Args...>::value;
};

template<int offset, class T, class... Args>
struct variadic_type_index<offset, T, T, Args...> {
    static constexpr int value = offset;
};

}

template<class T, class... Args>
using variadic_type_index = detail::variadic_type_index<0, T, Args...>;

template<class T, class... Args>
static constexpr int variadic_type_index_v = variadic_type_index<T, Args...>::value;

template<class Seq> struct size;
template<class Seq> struct front;
template<class Seq> struct back;
template<class Seq> struct pop_front;
template<int N, class Seq> struct get;
template<int N, class Seq> struct erase;
template<int Begin, int End, class Seq> struct slice;

template<class Seq> constexpr int size_v = size<Seq>::value;

template<class Seq> using front_t = typename front<Seq>::type;
template<class Seq> using back_t = typename back<Seq>::type;

template<class Seq> constexpr auto front_v = front<Seq>::value;
template<class Seq> constexpr auto back_v = back<Seq>::value;

template<class Seq> using pop_front_t = typename pop_front<Seq>::type;

template<int N, class Seq> constexpr auto get_v = get<N, Seq>::value;
template<int N, class Seq> using get_t = typename get<N, Seq>::type;
template<int N, class Seq> using erase_t = typename erase<N, Seq>::type;

template<int Begin, int End, class Seq>
using slice_t = typename slice<Begin, End, Seq>::type;

template<class T, class... Args>
struct any_of;

template<class T, class R, class... Args>
struct any_of<T, R, Args...> {
    static constexpr bool value = any_of<T, Args...>::value;
};

template<class T>
struct any_of<T> : std:: false_type {};

template<class T, class... Args>
struct any_of<T, T, Args...> : std::true_type {};

template<class T, class... Args>
static constexpr bool any_of_v = any_of<T, Args...>::value;

template<class T, class... Args>
static constexpr bool none_of_v = !any_of_v<T, Args...>;



template<class T>
struct _ {
//    template<template<class...> class F, class... Args>
//    using bind = F<T, Args...>;

    template<class... Args>
    using is_any_of = meta::any_of<T, Args...>;
    template<class... Args>
    static constexpr bool is_any_of_v = is_any_of<T, Args...>::value;
    template<class... Args>
    static constexpr bool is_none_of_v = !is_any_of_v<Args...>;


    template<class U>
    using eq = b0::meta::eq<T, U>;
    template<class U>
    static constexpr bool eq_v = eq<U>::value;



    template<class... Args>
    using is_constructible = std::is_constructible<T, Args...>;

    template<class Result>
    using can_construct = std::is_constructible<Result, T>;
};



template<class T>
struct is_tuple : meta::false_ {};
template<class... Args>
struct is_tuple<std::tuple<Args...>> : meta::true_ {};

template<class T>
constexpr bool is_tuple_v = is_tuple<T>::value;

template<class T1, class T2>
using tuple_cat = wrap_type<b0_autotype(std::tuple_cat(std::declval<T1>(), std::declval<T2>()))>;

template<class T1, class T2>
using tuple_cat_t = typename tuple_cat<T1, T2>::type;

}}
