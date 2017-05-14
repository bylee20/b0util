#pragma once

#include "./future.hpp"
#include "./thread_pool.hpp"
#include <future>

namespace b0 {

inline auto ideal_concurrency() -> int
{ return std::max<int>(1, static_cast<int>(std::thread::hardware_concurrency())); }

enum class run_policy { seq, par_sync, par_async };

template<run_policy p>
struct run_policy_t { static constexpr run_policy value = p; };

using run_seq_t = run_policy_t<run_policy::seq>;
using run_par_sync_t = run_policy_t<run_policy::par_sync>;
using run_par_async_t = run_policy_t<run_policy::par_async>;

constexpr run_seq_t run_seq{};
constexpr run_par_sync_t run_par_sync{};
constexpr run_par_async_t run_par_async{};

namespace meta {
template<class T> struct is_run_policy : false_ {};
template<run_policy p> struct is_run_policy<run_policy_t<p>> : true_ {};
}

template<class F, class... Args>
inline auto async(run_seq_t, F &&func, Args&&... args) -> auto
{
    META_ASSERT(b0::meta::is_invokable_v<F, Args...>);
    using T = std::decay_t<decltype(std::forward<F>(func)(std::forward<Args>(args)...))>;
    return future<T>::invoke_and_settle(std::forward<F>(func), std::forward<Args>(args)...);
}

template<class F, class... Args>
inline auto async(run_par_sync_t, F &&func, Args&&... args) -> auto
{
    META_ASSERT(b0::meta::is_invokable_v<F, Args...>);
    using T = std::decay_t<decltype(std::forward<F>(func)(std::forward<Args>(args)...))>;
    return future<T>::invoke_and_settle(std::forward<F>(func), std::forward<Args>(args)...);
}

template<class F, class... Args>
inline auto async(run_par_async_t, F &&func, Args&&... args) -> auto
{
    META_ASSERT(b0::meta::is_invokable_v<F, Args...>);
    using T = b0::meta::invoke_t<F&&, Args&&...>;
    b0::promise<T> promise;
    auto run = [promise, f=std::forward<F>(func),
                tuple=std::make_tuple(std::forward<Args>(args)...)] () mutable {
        promise.apply_and_settle(std::move(f), std::move(tuple));
    };
    std::thread task(run);
    task.detach();
    return promise.future();
}

template<class F, class... Args, B0_REQ(!meta::is_run_policy<F>::value)>
inline auto async(F &&func, Args&&... args) -> auto
{ return async(run_par_async, std::forward<F>(func), std::forward<Args>(args)...); }

}
