#pragma once

#include "./type_seq.hpp"
#include "./int_seq.hpp"

namespace b0 {namespace meta {namespace detail {

template<class MF, class T, class... Args>
inline constexpr auto invoke_helper(MF mf, T &&t, Args&&... args)
B0_RETURN_BODY((std::forward<T>(t).*mf)(std::forward<Args>(args)...))

template<class MF, class T, class... Args>
inline constexpr auto invoke_helper(MF mf, T &&t, Args&&... args)
B0_RETURN_BODY(((*std::forward<T>(t)).*mf)(std::forward<Args>(args)...))

template<class MF, class T, class... Args>
inline constexpr auto invoke_helper(MF mf, std::reference_wrapper<T> &&t, Args&&... args)
B0_RETURN_BODY((std::move(t).get().*mf)(std::forward<Args>(args)...))

template<class MF, class T, class... Args>
inline constexpr auto invoke_helper(MF mf, const std::reference_wrapper<T> &t, Args&&... args)
B0_RETURN_BODY((t.get().*mf)(std::forward<Args>(args)...))

template<class MF, class T, class... Args>
inline constexpr auto invoke_helper(MF mf, std::reference_wrapper<T> &t, Args&&... args)
B0_RETURN_BODY((t.get().*mf)(std::forward<Args>(args)...))

template<class Fn, class... Args>
inline constexpr auto invoke_helper(Fn &&fn, Args&&... args)
B0_RETURN_BODY(std::forward<Fn>(fn)(std::forward<Args>(args)...))

template<class Fn, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn)))

template<class Fn, class A1, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1)))

template<class Fn, class A1, class A2, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, A2 &&a2, Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1),
                            std::forward<A2>(a2)))

template<class Fn, class A1, class A2, class A3, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, A2 &&a2, A3 &&a3,
                                             Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1),
                            std::forward<A2>(a2), std::forward<A3>(a3)))

template<class Fn, class A1, class A2, class A3, class A4, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, A2 &&a2, A3 &&a3,
                                             A4 &&a4, Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1),
                            std::forward<A2>(a2), std::forward<A3>(a3),
                            std::forward<A4>(a4)))

template<class Fn, class A1, class A2, class A3, class A4, class A5, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, A2 &&a2, A3 &&a3,
                                             A4 &&a4, A5 &&a5, Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1),
                            std::forward<A2>(a2), std::forward<A3>(a3),
                            std::forward<A4>(a4), std::forward<A5>(a5)))

template<class Fn, class A1, class A2, class A3, class A4, class A5, class A6,
    class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, A2 &&a2, A3 &&a3,
                                             A4 &&a4, A5 &&a5, A6 &&a6, Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1),
                            std::forward<A2>(a2), std::forward<A3>(a3),
                            std::forward<A4>(a4), std::forward<A5>(a5),
                            std::forward<A6>(a6)))

template<class Fn, class A1, class A2, class A3, class A4, class A5, class A6,
    class A7, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, A2 &&a2, A3 &&a3,
                                             A4 &&a4, A5 &&a5, A6 &&a6, A7 &&a7,
                                             Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1),
                            std::forward<A2>(a2), std::forward<A3>(a3),
                            std::forward<A4>(a4), std::forward<A5>(a5),
                            std::forward<A6>(a6), std::forward<A7>(a7)))

template<class Fn, class A1, class A2, class A3, class A4, class A5, class A6,
    class A7, class A8, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, A2 &&a2, A3 &&a3,
                                             A4 &&a4, A5 &&a5, A6 &&a6, A7 &&a7,
                                             A8 &&a8, Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1),
                            std::forward<A2>(a2), std::forward<A3>(a3),
                            std::forward<A4>(a4), std::forward<A5>(a5),
                            std::forward<A6>(a6), std::forward<A7>(a7),
                            std::forward<A8>(a8)))

template<class Fn, class A1, class A2, class A3, class A4, class A5, class A6,
    class A7, class A8, class A9, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, A2 &&a2, A3 &&a3,
                                             A4 &&a4, A5 &&a5, A6 &&a6, A7 &&a7,
                                             A8 &&a8, A9 &&a9, Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1),
                            std::forward<A2>(a2), std::forward<A3>(a3),
                            std::forward<A4>(a4), std::forward<A5>(a5),
                            std::forward<A6>(a6), std::forward<A7>(a7),
                            std::forward<A8>(a8), std::forward<A9>(a9)))


template<class Fn, class A1, class A2, class A3, class A4, class A5, class A6,
    class A7, class A8, class A9, class A10, class... Args>
inline constexpr auto invoke_overflow_helper(Fn &&fn, A1 &&a1, A2 &&a2, A3 &&a3,
                                             A4 &&a4, A5 &&a5, A6 &&a6, A7 &&a7,
                                             A8 &&a8, A9 &&a9, A10 &&a10, Args&&...)
B0_RETURN_BODY(invoke_helper(std::forward<Fn>(fn), std::forward<A1>(a1),
                            std::forward<A2>(a2), std::forward<A3>(a3),
                            std::forward<A4>(a4), std::forward<A5>(a5),
                            std::forward<A6>(a6), std::forward<A7>(a7),
                            std::forward<A8>(a8), std::forward<A9>(a9),
                            std::forward<A10>(a10)))

template<int I, class... Args, B0_REQ((I < sizeof...(Args) && (1 > 0)))>
inline constexpr auto variadic_get(Args&&... args) noexcept -> decltype(auto)
{
    using fronts = slice_t<0, I, type_seq<std::decay_t<Args>...>>;
    return fronts::skip(std::forward<Args>(args)...);
}

}}}

