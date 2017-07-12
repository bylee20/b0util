#pragma once

#include "./_future_helper.hpp"
#include "./_future_state.hpp"
#include "../fp.hpp"
#include "../utility.hpp"
#include <exception>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>

namespace b0 {

template<class T, thread_safety ts>
class promise;

template<class T>
using promise_ts = promise<T, thread_safety::safe>;

namespace detail {

struct future_resolved_t {};

template<class F, class Arg, thread_safety ts>
struct result_to_future {
    using result_t = meta::invoke_overflow_t<F, Arg>;
    using type = meta::if_t<
        is_future<result_t>::value,
        result_t, b0::future<std::decay_t<result_t>, ts>
    >;
};

template<class T>
struct future_function : uncopyable {
    virtual ~future_function() = default;
    virtual auto invoke(T arg) -> void = 0;
};

template<class F, class R, class T, thread_safety ts>
struct future_function_impl : public future_function<T> {
    template<class _F>
    future_function_impl(b0::promise<R, ts> promise, _F &&f)
        : promise(std::move(promise)), func(std::forward<_F>(f)) {}
    auto invoke(T arg) -> void override
    {
        try {
            func(arg).propagate(promise);
        } catch (...) {
            promise.reject(std::current_exception());
        }
    }
    b0::promise<R, ts> promise;
    F func;
};

template<thread_safety>
struct future_thread_safe;

template<>
struct future_thread_safe<thread_safety::safe>
{
    future_thread_safe(): m_state(future_state::unsettled) {}
    future_thread_safe(future_state state)
        : m_state(state) { m_settled.test_and_set(); }
    auto check_settled() const -> bool { return m_settled.test_and_set(); }
    auto set_state(future_state state) -> void { m_state = state; }
    auto state() const -> future_state { return m_state; }
    auto lock() -> std::unique_ptr<std::lock_guard<std::mutex> >
    {
        return std::unique_ptr<std::lock_guard<std::mutex> >(new std::lock_guard<std::mutex>(m_mutex));
    }
    auto notify() -> void
    {
        m_condvar.notify_all();
    }
    auto wait() -> void
    {
        if (state() != future_state::unsettled)
            return;
        std::unique_lock<std::mutex> lock(m_mutex);
        if (state() != future_state::unsettled)
            return;
        m_condvar.wait(lock);
    }
private:
    mutable std::atomic_flag m_settled = ATOMIC_FLAG_INIT;
    std::atomic<future_state> m_state;
    std::mutex m_mutex;
    std::condition_variable m_condvar;
};

template<>
struct future_thread_safe<thread_safety::unsafe>
{
    future_thread_safe(): m_state(future_state::unsettled) {}
    future_thread_safe(future_state state): m_state(state) {}
    auto check_settled() const -> bool { return m_state != future_state::unsettled; }
    auto set_state(future_state state) -> void { m_state = state; }
    auto state() const -> future_state { return m_state; }
    auto lock() -> int { return 0; }
    auto notify() -> void {}
private:
    future_state m_state;
};

template<class T, thread_safety ts>
struct future_data : future_thread_safe<ts>, uncopyable
{
    template<class F, class Arg>
    using future_t = typename b0::detail::result_to_future<F, Arg, ts>::type;
    future_data() { }
    template<class U>
    future_data(future_resolved_t, U &&u)
        : future_thread_safe<ts>(future_state::resolved), m_data(std::forward<U>(u)) { }
    future_data(std::exception_ptr ex)
        : future_thread_safe<ts>(future_state::rejected), m_error(ex) { }

    template<class U>
    auto resolve(U &&u) -> void
    {
        _settle<future_state::resolved>([&] () {
            this->m_data = std::forward<U>(u);
            for (auto &&func : this->m_resolves)
                func->invoke(this->m_data);
        });
    }
    auto reject(std::exception_ptr e) -> void
    {
        _settle<future_state::rejected>([&] () {
            m_error = e;
            for (auto &&func : this->m_rejects)
                func->invoke(m_error);
        });
    }

    template<class OnResolved, class OnRejected>
    auto then(OnResolved &&onResolved, OnRejected &&onRejected) -> auto
    {
        using namespace b0::meta; using namespace b0::detail;
        META_ASSERT(is_invokable_v<decltype(onResolved), const T&>);
        META_ASSERT(is_invokable_v<decltype(onRejected), std::exception_ptr>);
        using resolved_t = invoke_t<decltype(onResolved), const T&>;
        using rejected_t = invoke_t<decltype(onRejected), std::exception_ptr>;
        META_ASSERT(is_future_v<resolved_t>);
        META_ASSERT(is_future_v<rejected_t>);
        META_ASSERT(eq_v<resolved_t, rejected_t>);
        using future_t = resolved_t;
        try {
#define CHECK_NOW() \
            switch (this->state()) { \
            case future_state::resolved: \
                return std::forward<OnResolved>(onResolved)(data()); \
            case future_state::rejected: \
                return std::forward<OnRejected>(onRejected)(error()); \
            default: break; }
            CHECK_NOW();
            auto locker = this->lock();
            (void)locker;
            CHECK_NOW();
            using type = typename future_t::value_type;
            b0::promise<type, ts> promise;
            using resolve_fun = future_function_impl<std::decay_t<OnResolved>, type, const T&, ts>;
            using reject_fun = future_function_impl<std::decay_t<OnRejected>, type, std::exception_ptr, ts>;
            this->m_resolves.emplace_back(new resolve_fun(promise, std::forward<OnResolved>(onResolved)));
            this->m_rejects.emplace_back(new reject_fun(promise, std::forward<OnRejected>(onRejected)));
            return promise.future();
        } catch(...) {
            return future_t::reject(std::current_exception());
        }
    }
    auto data() const -> const T& { return m_data; }
    auto _data() -> T& { return m_data; }
    auto error() const -> std::exception_ptr { return m_error; }
protected:
    template<future_state NewState, class SetData>
    auto _settle(SetData &&setData) -> void
    {
        if (this->check_settled())
            return;
        {
            auto locker = this->lock();
            (void)locker;
            setData();
            m_rejects.clear();
            m_resolves.clear();
            this->set_state(NewState);
        }
        this->notify();
    }

    std::exception_ptr m_error = nullptr;
    std::vector<std::unique_ptr<future_function<const T&>>> m_resolves;
    std::vector<std::unique_ptr<future_function<std::exception_ptr>>> m_rejects;
    T m_data;
};

}

}
