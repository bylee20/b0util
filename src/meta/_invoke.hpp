#pragma once

#include "./type_seq.hpp"
#include "./int_seq.hpp"

namespace b0 {

namespace meta {

namespace detail {

template<int I, class... Args>
inline constexpr auto variadic_get(Args&&... args) -> decltype(auto)
{
    using fronts = slice_t<0, I, type_seq<std::decay_t<Args>...>>;
    return fronts::skip(std::forward<Args>(args)...);
}

template<class F, class... Args, int... Ints>
inline constexpr auto try_invoke_impl(int, int_seq<Ints...>, F &&f, Args&&... args)
-> decltype(std::forward<F>(f)(variadic_get<Ints>(std::forward<Args>(args)...)...))
{ return std::forward<F>(f)(variadic_get<Ints>(std::forward<Args>(args)...)...); }

template<class F, class... Args>
inline constexpr auto try_invoke_impl(int, int_seq<>, F &&f, Args&&...)
-> decltype(std::forward<F>(f)())
{ return std::forward<F>(f)(); }

template<class F, class... Args, int... Ints, B0_REQ((sizeof...(Ints) < sizeof...(Args) && (1 > 0)))>
inline constexpr auto try_invoke_impl(char, int_seq<Ints...>, F &&f, Args&&... args)
-> decltype(try_invoke_impl(0, int_seq<Ints..., sizeof...(Ints)>(), std::forward<F>(f), std::forward<Args>(args)...))
{ return try_invoke_impl(0, int_seq<Ints..., sizeof...(Ints)>(), std::forward<F>(f), std::forward<Args>(args)...); }

template<class F, class... Args>
inline auto invoke_overflow_impl(F &&f, Args&&... args)
-> decltype(try_invoke_impl(0, meta::int_seq<>(), std::forward<F>(f), std::forward<Args>(args)...))
{
    return try_invoke_impl(0, meta::int_seq<>(), std::forward<F>(f), std::forward<Args>(args)...);
}

}

}

}
