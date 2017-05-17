#pragma once

#include <type_traits>
#include "./meta/meta.hpp"

namespace b0 {

template<class T>
inline constexpr auto max(T &&t) -> std::decay_t<T> { return std::forward<T>(t); }

template<class A1, class A2, class... Args>
inline constexpr auto max(A1 &&a1, A2 &&a2, Args&&... args)
-> std::common_type_t<A1&&, A2&&, Args&&...>
{
    return a1 > a2 ? b0::max(std::forward<A1>(a1), std::forward<Args>(args)...)
                   : b0::max(std::forward<A2>(a2), std::forward<Args>(args)...);
}

template<class LessThan, class T>
inline constexpr auto max_by(LessThan&&, T &&t) -> std::decay_t<T> { return std::forward<T>(t); }

template<class LessThan, class A1, class A2, class... Args>
inline constexpr auto max_by(LessThan &&lt, A1 &&a1, A2 &&a2, Args&&... args)
-> std::common_type_t<A1&&, A2&&, Args&&...>
{
    return std::forward<LessThan>(lt)(a2, a1)
            ? b0::max_by(std::forward<LessThan>(lt), std::forward<A1>(a1), std::forward<Args>(args)...)
            : b0::max_by(std::forward<LessThan>(lt), std::forward<A2>(a2), std::forward<Args>(args)...);
}

template<class T>
inline constexpr auto min(T &&t) -> std::decay_t<T> { return std::forward<T>(t); }

template<class A1, class A2, class... Args>
inline constexpr auto min(A1 &&a1, A2 &&a2, Args&&... args)
-> std::common_type_t<A1&&, A2&&, Args&&...>
{
    return a1 < a2 ? b0::min(std::forward<A1>(a1), std::forward<Args>(args)...)
                   : b0::min(std::forward<A2>(a2), std::forward<Args>(args)...);
}

template<class LessThan, class T>
inline constexpr auto min_by(LessThan&&, T &&t) -> std::decay_t<T> { return std::forward<T>(t); }

template<class LessThan, class A1, class A2, class... Args>
inline constexpr auto min_by(LessThan &&lt, A1 &&a1, A2 &&a2, Args&&... args)
-> std::common_type_t<A1&&, A2&&, Args&&...>
{
    return std::forward<LessThan>(lt)(a1, a2)
            ? b0::min_by(std::forward<LessThan>(lt), std::forward<A1>(a1), std::forward<Args>(args)...)
            : b0::min_by(std::forward<LessThan>(lt), std::forward<A2>(a2), std::forward<Args>(args)...);
}

namespace detail {

template<class T, class Min, class Max>
inline constexpr auto minmax_impl(Min &&min, Max &&max) -> std::pair<T, T>
{ return std::pair<T, T>(std::forward<Min>(min), std::forward<Max>(max)); }

template<class T, class Min, class Max, class A1, class... Args>
inline constexpr auto minmax_impl(Min &&min, Max &&max, A1&& a1, Args&&... args) -> auto
{
    return (a1 < min) ? minmax_impl<T>(std::forward<A1>(a1), std::forward<Max>(max), std::forward<Args>(args)...)
         : (a1 > max) ? minmax_impl<T>(std::forward<Min>(min), std::forward<A1>(a1), std::forward<Args>(args)...)
                      : minmax_impl<T>(std::forward<Min>(min), std::forward<Max>(max), std::forward<Args>(args)...);
}

template<class T, class LessThan, class Min, class Max>
inline constexpr auto minmax_by_impl(LessThan &&, Min &&min, Max &&max) -> std::pair<T, T>
{ return std::pair<T, T>(std::forward<Min>(min), std::forward<Max>(max)); }

template<class T, class LessThan, class Min, class Max, class A1, class... Args>
inline constexpr auto minmax_by_impl(LessThan &&lt, Min &&min, Max &&max, A1&& a1, Args&&... args) -> auto
{
    return lt(a1, min) ? minmax_by_impl<T>(std::forward<LessThan>(lt), std::forward<A1>(a1), std::forward<Max>(max), std::forward<Args>(args)...)
         : lt(max, a1) ? minmax_by_impl<T>(std::forward<LessThan>(lt), std::forward<Min>(min), std::forward<A1>(a1), std::forward<Args>(args)...)
                       : minmax_by_impl<T>(std::forward<LessThan>(lt), std::forward<Min>(min), std::forward<Max>(max), std::forward<Args>(args)...);
}

}

template<class T>
inline constexpr auto minmax(const T &t) -> auto { return std::make_pair(t, t); }

template<class A1, class A2, class... Args>
inline constexpr auto minmax(A1 &&a1, A2 &&a2, Args&&... args) -> auto
{
    using T = std::common_type_t<A1&&, A2&&, Args&&...>;
    return a1 < a2 ? detail::minmax_impl<T>(std::forward<A1>(a1), std::forward<A2>(a2), std::forward<Args>(args)...)
                   : detail::minmax_impl<T>(std::forward<A2>(a2), std::forward<A1>(a1), std::forward<Args>(args)...);
}

template<class LessThan, class T>
inline constexpr auto minmax_by(LessThan&&, const T &t) -> auto { return std::make_pair(t, t); }

template<class LessThan, class A1, class A2, class... Args>
inline constexpr auto minmax_by(LessThan &&lt, A1 &&a1, A2 &&a2, Args&&... args) -> auto
{
    using T = std::common_type_t<A1&&, A2&&, Args&&...>;
    return lt(a1, a2) ? detail::minmax_by_impl<T>(std::forward<LessThan>(lt), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<Args>(args)...)
                      : detail::minmax_by_impl<T>(std::forward<LessThan>(lt), std::forward<A2>(a2), std::forward<A1>(a1), std::forward<Args>(args)...);
}

template<class T, class A>
inline constexpr auto any_of(const T &t, const A &a) -> bool { return t == a; }

template<class T, class A, class... Args>
inline constexpr auto any_of(const T &t, const A &a, const Args&... args) -> bool
{ return t == a || b0::any_of(t, args...); }

template<class T, class... Args>
inline constexpr auto none_of(const T &t, const Args&... args) -> bool { return !any_of(t, args...); }

template<class T, class A>
inline constexpr auto all_of(const T &t, const A &a) -> bool { return t == a; }

template<class T, class A, class... Args>
inline constexpr auto all_of(const T &t, const A &a, const Args&... args) -> bool
{ return t == a && b0::all_of(t, args...); }

}
