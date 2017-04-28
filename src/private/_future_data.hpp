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

template<class T>
class promise;

namespace detail {

struct future_resolved_t {};

template<class F, class Arg>
struct result_to_future {
    using result_t = meta::invoke_overflow_t<F, Arg>;
    using type = meta::if_t<
        is_future<result_t>::value,
        result_t, b0::future<std::decay_t<result_t>>
    >;
};

template<class T>
struct future_function : uncopyable {
    virtual ~future_function() = default;
    virtual auto invoke(T arg) -> void = 0;
};

template<class F, class R, class T>
struct future_function_impl : public future_function<T> {
    template<class _F>
    future_function_impl(b0::promise<R> promise, _F &&f)
        : promise(std::move(promise)), func(std::forward<_F>(f)) {}
    auto invoke(T arg) -> void override
    {
        try {
            func(arg).propagate(promise);
        } catch (...) {
            promise.reject(std::current_exception());
        }
    }
    b0::promise<R> promise;
    F func;

};

template<class T>
struct future_data : uncopyable
{
    template<class F, class Arg>
    using future_t = typename b0::detail::result_to_future<F, Arg>::type;
    future_data(): m_state(future_state::unsettled) { }
    template<class U>
    future_data(future_resolved_t, U &&u)
        : m_state(future_state::resolved), m_data(std::forward<U>(u)) { m_settled.test_and_set(); }
    future_data(std::exception_ptr ex)
        : m_state(future_state::rejected), m_error(ex) { m_settled.test_and_set(); }

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
            switch (this->m_state) { \
            case future_state::resolved: \
                return std::forward<OnResolved>(onResolved)(data()); \
            case future_state::rejected: \
                return std::forward<OnRejected>(onRejected)(error()); \
            default: break; }
            CHECK_NOW();
            std::lock_guard<std::mutex> locker(this->m_mutex);
            CHECK_NOW();
            using type = typename future_t::value_type;
            b0::promise<type> promise;
            using resolve_fun = future_function_impl<std::decay_t<OnResolved>, type, const T&>;
            using reject_fun = future_function_impl<std::decay_t<OnRejected>, type, std::exception_ptr>;
            this->m_resolves.emplace_back(new resolve_fun(promise, std::forward<OnResolved>(onResolved)));
            this->m_rejects.emplace_back(new reject_fun(promise, std::forward<OnRejected>(onRejected)));
            return promise.future();
        } catch(...) {
            return future_t::reject(std::current_exception());
        }
    }
    auto data() const -> const T& { return m_data; }
    auto _data() -> T& { return m_data; }
    auto state() const -> future_state { return m_state.load(); }
    auto error() const -> std::exception_ptr { return m_error; }
    auto wait() -> void
    {
        if (state() != future_state::unsettled)
            return;
        std::unique_lock<std::mutex> lock(m_mutex);
        if (state() != future_state::unsettled)
            return;
        m_condvar.wait(lock);
    }
protected:
    template<future_state NewState, class SetData>
    auto _settle(SetData &&setData) -> void
    {
        if (m_settled.test_and_set())
            return;
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            setData();
            m_rejects.clear();
            m_resolves.clear();
            m_state = NewState;
        }
        m_condvar.notify_all();
    }

    std::atomic_flag m_settled = ATOMIC_FLAG_INIT;
    std::atomic<future_state> m_state;
    std::exception_ptr m_error = nullptr;
    std::vector<std::unique_ptr<future_function<const T&>>> m_resolves;
    std::vector<std::unique_ptr<future_function<std::exception_ptr>>> m_rejects;
    std::mutex m_mutex;
    std::condition_variable m_condvar;
    T m_data;
};

}

}
