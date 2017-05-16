#pragma once

#include "./_invoke.hpp"

namespace b0 {namespace meta {

namespace detail {

template<class F, class... Args>
class is_invokable_impl {
#pragma warning(push)
#pragma warning(disable: 4100)
    template<class _F, class... _Args> static auto test(int)
    -> decltype(invoke_helper(std::declval<_F>(), std::declval<_Args>()...), std::true_type{});
#pragma warning(pop)
    template<class _F, class... _Args> static auto test(...) -> std::false_type;
public:
    static constexpr bool value = std::decay_t<decltype(test<F, Args...>(0))>::value;
};

template<class F, class... Args>
class is_invokable_overflow_impl {
    template<class _F, class... _Args> static auto test(int)
    -> decltype((void)b0::meta::detail::invoke_overflow_helper(std::declval<_F>(), std::declval<_Args>()...), std::true_type{});
    template<class _F, class... _Args> static auto test(...) -> std::false_type;
public:
    static constexpr bool value = std::decay_t<decltype(test<F, Args...>(0))>::value;
};

template<bool, class F, class... Args>
struct invoke_result {};

template<class F, class... Args>
struct invoke_result<true, F, Args...>
{ using type = decltype(b0::meta::detail::invoke_helper(std::declval<F>(), std::declval<Args>()...)); };

template<bool, class F, class... Args>
struct invoke_overflow_result {};

template<class F, class... Args>
struct invoke_overflow_result<true, F, Args...>
{ using type = decltype(b0::meta::detail::invoke_overflow_helper(std::declval<F>(), std::declval<Args>()...)); };

}

template<class F, class... Args>
using is_invokable = bool_<meta::detail::is_invokable_impl<F, Args...>::value>;

template<class F, class... Args>
using is_invokable_overflow = bool_<meta::detail::is_invokable_overflow_impl<F, Args...>::value>;

template<class F, class... Args>
constexpr bool is_invokable_v = is_invokable<F, Args...>::value;

template<class F, class... Args>
constexpr bool is_invokable_overflow_v = is_invokable_overflow<F, Args...>::value;

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

}}
