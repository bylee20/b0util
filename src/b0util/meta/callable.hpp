#pragma once

#include "./_invoke.hpp"

namespace b0 {

namespace meta {

namespace detail {

template<class F, class... Args>
class is_invokable_impl {
#pragma warning(push)
#pragma warning(disable: 4100)
    template<class _F, class... _Args> static auto test(int)
    -> decltype((void)std::declval<_F>()(std::declval<_Args>()...), std::true_type{});
#pragma warning(pop)
    template<class _F, class... _Args> static auto test(...) -> std::false_type;
public:
    static constexpr bool value = decltype(test<F, Args...>(0))::value;
};

template<class F, class... Args>
class is_invokable_overflow_impl {
    template<class _F, class... _Args> static auto test(int)
    -> decltype(b0::meta::detail::invoke_overflow_impl(std::declval<_F>(), std::declval<_Args>()...), std::true_type{});
    template<class _F, class... _Args> static auto test(...) -> std::false_type;
public:
    static constexpr bool value = decltype(test<F, Args...>(0))::value;
};

}

template<class F, class... Args>
using is_invokable = bool_<meta::detail::is_invokable_impl<F, Args...>::value>;

template<class F, class... Args>
using is_invokable_overflow = bool_<meta::detail::is_invokable_overflow_impl<F, Args...>::value>;

template<class F, class... Args>
constexpr bool is_invokable_v = is_invokable<F, Args...>::value;

template<class F, class... Args>
constexpr bool is_invokable_overflow_v = is_invokable_overflow<F, Args...>::value;

namespace detail {

template<bool, class F, class... Args>
struct invoke_result {};

template<class F, class... Args>
struct invoke_result<true, F, Args...>
{ using type = decltype(std::declval<F>()(std::declval<Args>()...)); };

template<bool, class F, class... Args>
struct invoke_overflow_result {};

template<class F, class... Args>
struct invoke_overflow_result<true, F, Args...>
{ using type = decltype(b0::meta::detail::invoke_overflow_impl(std::declval<F>(), std::declval<Args>()...)); };

}

template<class F, class... Args>
using invoke = detail::invoke_result<is_invokable_v<F, Args...>, F, Args...>;

template<class F, class... Args>
using invoke_overflow = detail::invoke_overflow_result<is_invokable_overflow_v<F, Args...>, F, Args...>;

template<class F, class... Args>
using invoke_t = typename invoke<F, Args...>::type;

template<class F, class... Args>
using invoke_overflow_t = typename invoke_overflow<F, Args...>::type;

template<class F, class Tuple>
struct is_applicable { static constexpr bool value = false; };

template<class F, class... Args>
struct is_applicable<F, std::tuple<Args...>> : public is_invokable<F, Args...> {};

template<class F, class Tuple>
struct is_applicable_overflow { static constexpr bool value = false; };

template<class F, class... Args>
struct is_applicable_overflow<F, std::tuple<Args...>> : public is_invokable_overflow<F, Args...> {};

template<class F, class Tuple>
constexpr bool is_applicable_v = is_applicable<F, Tuple>::value;

template<class F, class Tuple>
constexpr bool is_applicable_overflow_v = is_applicable_overflow<F, Tuple>::value;

template<class F, class Tuple>
struct apply {};

template<class F, class... Args>
struct apply<F, std::tuple<Args...>> : public invoke<F, Args...> {};

template<class F, class Tuple>
struct apply_overflow {};

template<class F, class... Args>
struct apply_overflow<F, std::tuple<Args...>> : public invoke_overflow<F, Args...> {};

template<class F, class Tuple>
using apply_t = typename apply<F, Tuple>::type;

template<class F, class Tuple>
using apply_overflow_t = typename apply_overflow<F, Tuple>::type;

}

}


//namespace b0 {
//namespace meta {
//namespace detail {

////template<int N, class... Args>
////constexpr auto variadic_get(Args&&... args) -> decltype(auto)
////{ return split<0, N, type_seq<decltype(std::forward<Args>(args))...>>::skip(std::forward<Args>(args)...); }

////template<class F, class... Args, int... at>
////B0_SCI auto invoke_resolve(int, int_seq<at...>, F &&f, Args&&... args)
////-> decltype(std::forward<F>(f)(::b0::get<at>(std::forward<Args>(args)...)...))
////{ return std::forward<F>(f)(::b0::get<at>(std::forward<Args>(args)...)...); }

////template<class F, class... Args, int... at>
////B0_SCI auto invoke_resolve(B0_REQUIRES_TYPE(char, (sizeof...(at) < sizeof...(Args))),
////                           int_seq<at...>, F &&f, Args&&... args) -> decltype(auto)
////{ return invoke_resolve(0, int_seq<at..., sizeof...(at)>(), std::forward<F>(f), std::forward<Args>(args)...); }


//template <bool callable, class Ret, class...Args>
//struct callable_traits_base {
//    static constexpr int arity = sizeof...(Args);
//    static constexpr bool is_callable = callable;
//    using arguments_tuple_type = std::tuple<Args...>;
//    using args_seq = type_seq<Args...>;
//    using return_type = Ret;
////    template <int i>
////    using arg_t = typename func_get_arg_s<i, i < arity, Args...>::type;
////    using arg_idx_seq = std::index_sequence_for<Args...>;
////    using function = std::function<Ret(Args...)>;
//};

//template <class Ret, class...Args>
//struct callable_traits_impl : callable_traits_base<true, Ret, Args...> {};

//template <class T>
//struct functor_traits; // never instantiated
//template <class T, class Ret, class... Args>
//struct functor_traits<Ret(T::*)(Args...) const> : callable_traits_impl<Ret, Args...> {};
//template <class T, class Ret, class... Args>
//struct functor_traits<Ret(T::*)(Args...)> : callable_traits_impl<Ret, Args...> {};

//}

//template <class T, bool = is_functor_v<T>, bool is_generic = false>
//struct callable_traits : detail::callable_traits_base<false, void> {};
//template<class T>
//struct callable_traits<T, true> : detail::functor_traits<decltype(&uncref_t<T>::operator())> {};
//template <class Ret, class... Args>
//struct callable_traits<Ret(Args...), false> : detail::callable_traits_impl<Ret, Args...> {};
//template <class Ret, class... Args>
//struct callable_traits<Ret(*)(Args...), false> : detail::callable_traits_impl<Ret, Args...> {};

//template<class T>
//B0_SC int arity_v = callable_traits<T>::arity;
//template<class T>
//using ret_t = typename callable_traits<T>::return_type;
//template<int N, class T>
//using arg_t = std::tuple_element_t<N, typename callable_traits<T>::arguments_tuple_type>;
//template<class T>
//using args_seq_t = typename callable_traits<T>::args_seq;
////template<class T>
////B0_SC bool is_callable_v = callable_traits<T>::is_callable;
//template<class T>
//B0_SC bool can_return_v = std::is_same_v<void, ret_t<T>>;

//}
//}


////namespace b0 {
////namespace detail {
////template <typename T>
////struct has_parenthesis_operator
////{
////    template<typename C>
////    static char test(decltype(&C::operator()));
////    template<typename C>
////    static int test(...);
////    static const bool value = sizeof(test<T>(0)) == 1;
////};

////}

////// Traits on callable (function, functors and lambda functions).

////// callable_traits<F>::is_callable = true_type if F is callable.
////// callable_traits<F>::arity = N if F takes N arguments.
////// callable_traits<F>::arguments_tuple_type = tuple<Arg1, ..., ArgN>

////template <typename F, typename X = void>
////struct callable_traits
////{
////    typedef std::false_type is_callable;
////    static const int arity = 0;
////    typedef std::tuple<> arguments_tuple;
////    typedef typelist<> arguments_list;
////    typedef void return_type;
////};

////template <typename F, typename X>
////struct callable_traits<F&, X> : public callable_traits<F, X> {};
////template <typename F, typename X>
////struct callable_traits<F&&, X> : public callable_traits<F, X> {};
////template <typename F, typename X>
////struct callable_traits<const F&, X> : public callable_traits<F, X> {};

////template <typename F>
////struct callable_traits<F, std::enable_if_t<internal::has_parenthesis_operator<F>::value>>
////{
////    typedef callable_traits<decltype(&F::operator())> super;
////    typedef std::true_type is_callable;
////    static const int arity = super::arity;
////    typedef typename super::arguments_tuple arguments_tuple;
////    typedef typename super::arguments_list arguments_list;
////    typedef typename super::return_type return_type;
////};

////template <typename C, typename R, typename... ARGS>
////struct callable_traits<R (C::*)(ARGS...) const>
////{
////    typedef std::true_type is_callable;
////    static const int arity = sizeof...(ARGS);
////    typedef std::tuple<ARGS...> arguments_tuple;
////    typedef typelist<ARGS...> arguments_list;
////    typedef R return_type;
////};

////template <typename C, typename R, typename... ARGS>
////struct callable_traits<R (C::*)(ARGS...)>
////{
////    typedef std::true_type is_callable;
////    static const int arity = sizeof...(ARGS);
////    typedef std::tuple<ARGS...> arguments_tuple;
////    typedef typelist<ARGS...> arguments_list;
////    typedef R return_type;
////};

////template <typename R, typename... ARGS>
////struct callable_traits<R(ARGS...)>
////{
////    typedef std::true_type is_callable;
////    static const int arity = sizeof...(ARGS);
////    typedef std::tuple<ARGS...> arguments_tuple;
////    typedef typelist<ARGS...> arguments_list;
////    typedef R return_type;
////};


////template <typename R, typename... ARGS>
////struct callable_traits<R(*)(ARGS...)>
////{
////    typedef std::true_type is_callable;
////    static const int arity = sizeof...(ARGS);
////    typedef std::tuple<ARGS...> arguments_tuple;
////    typedef typelist<ARGS...> arguments_list;
////    typedef R return_type;
////};

////template <typename F>
////using callable_arguments_tuple_t = typename callable_traits<F>::arguments_tuple;
////template <typename F>
////using callable_arguments_list_t = typename callable_traits<F>::arguments_list;
////template <typename F>
////using callable_return_type_t = typename callable_traits<F>::return_type;

////template <typename F>
////struct is_callable : public callable_traits<F>::is_callable {};

////template <typename F, typename... A>
////struct callable_with
////{
////    template<typename G, typename... B>
////    static char test(int x, std::remove_reference_t<decltype(std::declval<G>()(std::declval<B>()...))>* = 0);
////    template<typename G, typename... B>
////    static int test(...);
////    static const bool value = sizeof(test<F, A...>(0)) == 1;
////};

////}
