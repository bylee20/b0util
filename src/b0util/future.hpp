#pragma once

#include "./private/_future_state.hpp"
#include "./private/_future_data.hpp"
#include "./private/_future_helper.hpp"
#include <memory>
#include <tuple>

namespace b0
{

template<class T, thread_safety ts>
class future;

class promise_error : public std::runtime_error {
public:
    promise_error(const char *msg): std::runtime_error(msg) {}
    promise_error(const std::string &msg): std::runtime_error(msg) {}
    promise_error(): std::runtime_error("general promise error") {}
};

template<class T>
using promise_safe = promise<T, thread_safety::safe>;
template<class T>
using promise_unsafe = promise<T, thread_safety::unsafe>;

template<class T, thread_safety ts = thread_safety::safe>
class promise {
public:
    promise(): m_data(new b0::detail::future_data<T, ts>()) { }
    auto future() const -> auto { return b0::future<T, ts>(m_data); }
    template<class U>
    auto resolve(U &&u) const -> void { m_data->resolve(std::forward<U>(u)); }
    template<class E>
    auto reject(E &&e) const -> void
    { reject(std::make_exception_ptr(std::forward<E>(e))); }
    auto reject(std::exception_ptr e) const -> void { m_data->reject(e); }
    auto reject() const -> void { reject(promise_error()); }
    template<class F, class... Args>
    auto invoke_and_settle(F &&func, Args&&... args) -> void
    {
        try {
            resolve(b0::invoke(std::forward<F>(func), std::forward<Args>(args)...));
        } catch (...) {
            reject(std::current_exception());
        }
    }
    template<class F, class Tuple>
    auto apply_and_settle(F &&func, Tuple &&tuple) -> void
    {
        try {
            resolve(b0::apply(std::forward<F>(func), std::forward<Tuple>(tuple)));
        } catch (...) {
            reject(std::current_exception());
        }
    }
protected:
    std::shared_ptr<b0::detail::future_data<T, ts> > m_data;
};

template<class T>
using future_safe = future<T, thread_safety::safe>;
template<class T>
using future_unsafe = future<T, thread_safety::unsafe>;

template<class T, thread_safety ts = thread_safety::safe>
class future {
    B0_USING_REQ_T;
    static constexpr bool is_void = std::is_void_v<T>;
    using data_t = b0::detail::future_data<T, ts>;
    META_ASSERT(!is_future_v<T>);
    META_ASSERT(!meta::eq_v<T, std::exception_ptr>);
public:
    using value_type = T;

    future(): m_data(new data_t()) { }
    future(future&&) = default;
    future(const future&) = default;
    auto operator = (future&&) -> future& = default;
    auto operator = (const future&) -> future& = default;

    template<class U>
    static auto resolve(U &&u) -> future
    { return future(std::make_shared<data_t>(detail::future_resolved_t(), std::forward<U>(u))); }

    static auto reject(std::exception_ptr e) -> future<T, ts>
    { return future<T, ts>(std::make_shared<data_t>(e)); }

    template<class F, class... Args>
    static auto invoke_and_settle(F &&func, Args&&... args) -> future
    {
        try {
            return resolve(b0::invoke(std::forward<F>(func), std::forward<Args>(args)...));
        } catch (...) {
            return reject(std::current_exception());
        }
    }

    template<class F, class Tuple>
    inline auto apply_and_settle(F &&func, Tuple &&tuple) -> future
    {
        try {
            return resolve(b0::apply(std::forward<F>(func), std::forward<Tuple>(tuple)));
        } catch (...) {
            return reject(std::current_exception());
        }
    }

    constexpr auto is_settled() const -> bool
    { return m_data->state() != future_state::unsettled; }
    constexpr auto is_resolved() const -> bool
    { return m_data->state() == future_state::resolved; }
    constexpr auto is_rejected() const -> bool
    { return m_data->state() == future_state::rejected; }

    template<class OnResolved, class OnRejected>
    auto then(OnResolved &&onResolved, OnRejected &&onRejected) const -> auto
    {
        using b0::detail::future_wrap;
        return m_data->then(future_wrap<ts, const T&>(std::forward<OnResolved>(onResolved)),
                            future_wrap<ts, std::exception_ptr>(std::forward<OnRejected>(onRejected)));
    }
    template<class F>
    auto then(F &&func) const -> auto
    {
        auto onRejected = [] (std::exception_ptr e) {
            using ft = typename data_t::template future_t<F, const T&>;
            return ft::reject(e);
        };
        return then(std::forward<F>(func), std::move(onRejected));
    }

    auto wait() const -> void { m_data->wait(); }
    auto get() const -> const T&
    {
        wait();
        if (is_resolved())
            return m_data->data();
        else if (is_rejected())
            std::rethrow_exception(m_data->error());
        else
            throw std::logic_error("future::get(): unable state");
    }

    auto propagate(promise<T, ts> promise) const -> future<T, ts>
    {
        try {
            switch (m_data->state()) {
            case future_state::resolved:
                promise.resolve(m_data->data());
                break;
            case future_state::rejected:
                promise.reject(m_data->error());
                break;
            default:
                return m_data->then([promise] (const T &t) {
                    promise.resolve(t);
                    return promise.future();
                }, [promise] (std::exception_ptr e) {
                    promise.reject(e);
                    return promise.future();
                });
                break;
            }
        } catch(...) {
            promise.reject(std::current_exception());
        }
        return promise.future();
    }

    template<class F>
    auto fail(F &&func) const -> auto
    {
        auto onResolved = [] (const T &) {
            using future_t = typename data_t::template future_t<F, std::exception_ptr>;
            return future_t::resolve();
        };
        return then(std::move(onResolved), std::forward<F>(func));
    }

    template<class U, class F>
    auto fail_or(F &&func, U &&u = {}) const -> auto
    {
        auto onResolved = [u=std::forward<U>(u)] (const T &) {
            using future_t = typename data_t::template future_t<F, std::exception_ptr>;
            return future_t::resolve(std::move(u));
        };
        return then(std::move(onResolved), std::forward<F>(func));
    }
protected:
    friend class promise<T, ts>;
    future(std::shared_ptr<data_t> data): m_data(std::move(data)) { }
    std::shared_ptr<data_t> m_data;
};

template<thread_safety ts>
class future<void, ts> : public future<meta::void_t, ts>
{
public:
    future<void, ts>() {}
    future<void, ts>(future<meta::void_t, ts> &&other): future<meta::void_t, ts>(std::move(other)) {}
    future<void, ts>(const future<meta::void_t, ts> &other): future<meta::void_t, ts>(other) {}
    static auto resolve() -> future<meta::void_t, ts> { return future<meta::void_t, ts>::resolve(meta::void_t()); }
    template<class F, class... Args>
    inline auto make_invoke_and_settle(F &&func, Args&&... args) -> auto
    {
        try {
            b0::invoke(std::forward<F>(func), std::forward<Args>(args)...);
            return resolve();
        } catch (...) {
            return reject(std::current_exception());
        }
    }

    template<class F, class Tuple>
    inline auto make_apply_and_settle(F &&func, Tuple &&tuple) -> auto
    {
        try {
            b0::apply(std::forward<F>(func), std::forward<Tuple>(tuple));
            return resolve();
        } catch (...) {
            return reject(std::current_exception());
        }
    }
};

template<thread_safety ts>
class promise<void, ts> : public promise<meta::void_t, ts>
{
public:
    constexpr auto resolve() const -> void { promise<meta::void_t, ts>::resolve(meta::void_t()); }
    template<class F, class... Args>
    auto invoke_and_settle(F &&func, Args&&... args) -> void
    {
        try {
            b0::invoke(std::forward<F>(func), std::forward<Args>(args)...);
            resolve();
        } catch (...) {
            reject(std::current_exception());
        }
    }
    template<class F, class Tuple>
    auto apply_and_settle(F &&func, Tuple &&tuple) -> void
    {
        try {
            b0::apply(std::forward<F>(func), std::forward<Tuple>(tuple));
            resolve();
        } catch (...) {
            reject(std::current_exception());
        }
    }
};

template<class T>
inline constexpr auto make_resolve(T &&t) -> auto
{
    return future<std::decay_t<T>>::resolve(std::forward<T>(t));
}

template<class T, class E>
inline constexpr auto make_reject(E &&e) -> auto
{
    return future<T>::reject(std::make_exception_ptr(std::forward<E>(e)));
}

template<class T>
inline auto make_reject(std::exception_ptr e) -> auto
{
    return future<T>::reject(e);
}

template<class F, class... Args>
inline auto make_invoke_and_settle(F &&func, Args&&... args) -> auto
{
    using T = std::decay_t<decltype(b0::invoke(std::forward<F>(func), std::forward<Args>(args)...))>;
    return future<T>::invoke_and_settle(std::forward<F>(func), std::forward<Args>(args)...);
}

template<class F, class Tuple>
inline auto make_apply_and_settle(F &&func, Tuple &&tuple) -> auto
{
    using T = std::decay_t<decltype(b0::apply(std::forward<F>(func), std::forward<Tuple>(tuple)))>;
    return future<T>::apply_and_settle(std::forward<F>(func), std::forward<Tuple>(tuple));
}

namespace detail {

template<int I, int N, bool = (I < N && 1 > 0)>
struct future_all_s {
    template<thread_safety ts, class Tuple, class T, class... Args>
    static auto apply(b0::promise<Tuple, ts> &promise, std::shared_ptr<Tuple> tuple,
                      std::shared_ptr<std::atomic_int32_t> counter,
                      const b0::future<T, ts> &future, const b0::future<Args, ts>&... args) -> void
    {
        future.then([=] (const b0::meta::wrap_void_t<T> &t) {
            std::get<I>(*tuple) = t;
            if (N == ++*counter)
                promise.resolve(std::move(*tuple));
        }, [promise] (std::exception_ptr e) {
            promise.reject(e);
        });
        future_all_s<I + 1, N>::apply(promise, tuple, counter, args...);
    }
};

template<int I, int N>
struct future_all_s<I, N, false> {
    template<thread_safety ts, class Tuple>
    static auto apply(b0::promise<Tuple, ts> &, std::shared_ptr<Tuple>, std::shared_ptr<std::atomic_int32_t>) -> void {}
};

}

template<thread_safety ts, class... Args>
auto future_all(future<Args, ts>... futures) -> auto
{
    using tuple_type = std::tuple<meta::wrap_void_t<Args>...>;
    auto counter = std::make_shared<std::atomic_int32_t>(0);
    auto tuple = std::make_shared<tuple_type>();
    b0::promise<tuple_type, ts> promise;
    b0::detail::future_all_s<0, sizeof...(Args)>::apply(promise, tuple, counter, futures...);
    return promise.future();
}

template<thread_safety ts, class T>
auto future_all(std::initializer_list<b0::future<T, ts> > futures) -> auto
{
    using type = meta::wrap_void_t<T>;
    auto results = std::make_shared<std::vector<type> >(futures.size());
    auto counter = std::make_shared<std::atomic_int32_t>(0);
    const int len = static_cast<int>(futures.size());
    b0::promise<std::vector<type>, ts> promise;
    auto it = futures.begin();
    for (int i = 0; i < len; ++i, ++it) {
        it->then([=] (const type &t) {
            (*results)[i] = t;
            if (++*counter == len)
                promise.resolve(std::move(*results));
        }, [=] (std::exception_ptr e) {
            promise.reject(e);
        });
    }
    return promise.future();
}

template<class T>
inline auto unpack(std::exception_ptr e) -> T
{
    try {
        std::rethrow_exception(e);
    } catch (const T &v) {
        return v;
    } catch (...) {
        throw;
    }
}

}
