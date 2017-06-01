#pragma once

#include "../meta/meta.hpp"
#include "../meta/callable.hpp"
#include "../fp.hpp"
#include "../utility.hpp"

namespace b0 {

template<class T, thread_safety ts>
class future;

template<class T>
struct is_future : meta::bool_<false> {};

template<class T, thread_safety ts>
struct is_future<future<T, ts>> : meta::bool_<true> {};

template<class T, thread_safety ts>
struct to_future : meta::wrap_type<future<T, ts>>
{
    template<class U>
    static constexpr auto apply(U &&u) -> future<T, ts> { return future<T, ts>::resolve(std::forward<U>(u)); }
    static auto apply(std::exception_ptr e) -> future<T, ts> { return future<T, ts>::reject(e); }
};

template<class T, thread_safety ts>
struct to_future<future<T, ts>, ts> : to_future<T, ts>
{
    static constexpr auto apply(future<T, ts> f) -> future<T, ts> { return f; }
};

template<class T>
constexpr bool is_future_v = is_future<T>::value;

template<class T, thread_safety ts>
using to_future_t = typename to_future<T, ts>::type;

namespace detail {

using namespace meta;
using std::is_void_v;

template<thread_safety ts, class Arg, class R, class F>
inline constexpr auto future_wrap_impl(F &&func,
    req_t<!is_void_v<Arg> && is_void_v<R>, void*> = nullptr) -> auto
{
    using void_t = wrap_void_t<R>;
    static_assert(is_invokable_overflow_v<F&&, Arg>, "cannot invoke");
    return [f=std::forward<F>(func)] (Arg arg) {
        b0::invoke_overflow(f, arg);
        return b0::future<void_t, ts>::resolve(void_t());
    };
}

template<thread_safety ts, class Arg, class R, class F>
inline constexpr auto future_wrap_impl(F &&func,
    req_t<!is_void_v<Arg> && !is_void_v<R>, void*> = nullptr) -> auto
{
    static_assert(is_invokable_overflow_v<F&&, Arg>, "cannot invoke");
    return [f=std::forward<F>(func)] (Arg arg) {
//        return future<R>();
        return to_future<R, ts>::apply(b0::invoke_overflow(f, arg));
    };
}

template<thread_safety ts, class Arg, class F>
inline constexpr auto future_wrap(F &&func) -> auto
{
    using result_t = invoke_overflow_t<F&&, Arg>;
    return future_wrap_impl<ts, Arg, result_t>(std::forward<F>(func));
}

}

}
