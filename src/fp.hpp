#pragma once

//#include "./tmp.hpp"
//#include "./callable_traits.hpp"
#include "./meta/type_seq.hpp"
#include "./meta/int_seq.hpp"
#include "./meta/callable.hpp"

namespace b0 {

template<int I, class... Args>
inline constexpr auto get(Args&&... args)
-> decltype(meta::detail::variadic_get<I>(std::forward<Args>(args)...))
{
    return meta::detail::variadic_get<I>(std::forward<Args>(args)...);
}

template<class F, class... Args>
inline constexpr auto invoke(F &&f, Args&&... args) -> decltype(auto)
{
    META_ASSERT(meta::is_invokable_v<decltype(f), decltype(args)...>);
    return std::forward<F>(f)(std::forward<Args>(args)...);
}

namespace detail {

template<class F, class... Args>
inline constexpr auto invoke_overflow_impl(meta::bool_<true>, F &&f, Args&&... args) -> decltype(auto)
{
    return std::forward<F>(f)(std::forward<Args>(args)...);
}

template<class F, class... Args>
inline constexpr auto invoke_overflow_impl(meta::bool_<false>, F &&f, Args&&... args) -> decltype(auto)
{
    return meta::detail::invoke_overflow_impl(std::forward<F>(f), std::forward<Args>(args)...);
}

}

template<class F, class... Args>
inline constexpr auto invoke_overflow(F &&f, Args&&... args) -> decltype(auto)
{
    META_ASSERT(meta::is_invokable_overflow_v<decltype(f), decltype(args)...>);
    return b0::detail::invoke_overflow_impl(meta::is_invokable<decltype(f), decltype(args)...>(),
                                            std::forward<F>(f), std::forward<Args>(args)...);
}

namespace detail {

using namespace b0::meta;

template<class F, class Tuple, int... Ints>
inline constexpr auto apply_impl(F &&f, Tuple &&tuple, int_seq<Ints...>) -> decltype(auto)
{
    return std::forward<F>(f)(std::get<Ints>(std::forward<Tuple>(tuple))...);
}

template<class F, class Tuple, int... Ints>
inline constexpr auto apply_overflow_impl(F &&f, Tuple &&tuple, int_seq<Ints...>) -> decltype(auto)
{
    return b0::invoke_overflow(std::forward<F>(f), std::get<Ints>(std::forward<Tuple>(tuple))...);
}

}

template<class F, class Tuple>
inline constexpr auto apply(F &&f, Tuple &&tuple) -> decltype(auto)
{
    using b0::detail::apply_impl;
    return apply_impl(std::forward<F>(f), std::forward<Tuple>(tuple),
                      meta::make_int_seq<std::tuple_size_v<std::decay_t<Tuple>>>());
}

template<class F, class Tuple>
inline constexpr auto apply_overflow(F &&f, Tuple &&tuple) -> decltype(auto)
{
    using b0::detail::apply_overflow_impl;
    return apply_overflow_impl(std::forward<F>(f), std::forward<Tuple>(tuple),
                               meta::make_int_seq<std::tuple_size_v<std::decay_t<Tuple>>>());
}

namespace detail {

template<class F, class... Args, int... Ints>
constexpr auto for_each_impl(meta::int_seq<Ints...>, Args&&... args, const F &f) -> void
{ b0_expand(f(b0::get<Ints>(std::forward<Args>(args)...))); }

template<class F, class T>
constexpr inline auto for_each_if_impl(T &&t, const F &f) -> bool
{ return f(std::forward<T>(t)); }

template<class F, class T, class... Args>
constexpr inline auto for_each_if_impl(T &&t, Args&&... args, const F &f) -> bool
{ return f(std::forward<T>(t)) && for_each_if_impl(std::forward<Args>(args)..., f); }

}

template<class... Args, class F>
constexpr inline auto for_each(Args&&... args, const F &f) -> void
{ detail::for_each_impl(meta::int_seq_for<Args...>(), std::forward<Args>(args)..., f); }

template<class... Args, class F>
constexpr inline auto for_each_if(Args&&... args, const F &f) -> bool
{ return detail::for_each_if_impl(std::forward<Args>(args)..., f); }

}
