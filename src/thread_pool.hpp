#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <deque>
#include "./future.hpp"

namespace b0 {

class thread_pool : uncopyable
{
    struct job_t : uncopyable {
        virtual ~job_t() = default;
        virtual auto invoke() -> void = 0;
    };
    template<class F, class R, class... Args>
    struct job_impl_t : job_t {
        template<class _F, class... _Args>
        job_impl_t(b0::promise<R> promise, _F &&func, _Args&&... args)
            : promise(std::move(promise)), func(std::forward<_F>(func))
            , args(std::forward<_Args>(args)...) { }
        auto invoke() -> void override
        { promise.apply_and_settle(std::move(func), std::move(args)); }
    private:
        b0::promise<R> promise; F func;
        std::tuple<Args...> args;
    };
public:
    thread_pool();
    thread_pool(int count);
    thread_pool(thread_pool&&) = default;
    ~thread_pool();
    static auto ideal_size() -> int;
    auto operator = (thread_pool&&) -> thread_pool& = default;
    auto size() const -> int;
    template<class F, class... Args>
    auto start(F &&func, Args&&... args)
    -> future<std::decay_t<decltype(std::forward<F>(func)(std::forward<Args>(args)...))> >
    {
        using R = std::decay_t<decltype(std::forward<F>(func)(std::forward<Args>(args)...))>;
        using T = job_impl_t<R, std::decay_t<F>, std::decay_t<Args>...>;
        b0::promise<R> promise;
        _enqueue(std::unique_ptr<job_t>(new T(promise, std::forward<F>(func), std::forward<Args>(args)...)));
        return promise.future();
    }
private:
    auto _enqueue(std::unique_ptr<job_t> &&job) -> void;
    struct data;
    std::unique_ptr<data> d;
};

}
