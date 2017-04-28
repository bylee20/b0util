#pragma once

#include "./_terminal_fn.hpp"

namespace b0 {

namespace range {

template<class T, class Policy, class Fn>
struct reduce_fn_base : terminal_fn {
    using run_policy = Policy;

    template<class F>
    constexpr reduce_fn_base(in_place_t, F &&fn)
        : m_fn(std::forward<Fn>(fn)) {}
    template<class Iterable>
    auto run(Iterable &&it) const -> auto
    {
        auto acc = static_cast<const T&>(*this).get_init(it);
        for (; !it.at_end(); it.next())
            m_fn(acc, it.get());
        return acc;
    }
    template<class T>
    auto parallel(std::vector<T> &&values) const -> T
    {
        if (values.empty())
            return T();
        auto it = values.begin();
        auto init = *it;
        for (++it; it != values.end(); ++it)
            m_fn(init, std::move(*it));
        return init;
    }
protected:
    Fn m_fn;
};

template<class Policy, class Fn, class Init>
struct reduce_fn : reduce_fn_base<reduce_fn<Policy, Fn, Init>, Policy, Fn> {
    using super = reduce_fn_base<reduce_fn<Policy, Fn, Init>, Policy, Fn>;
    template<class F, class I>
    constexpr reduce_fn(in_place_t, F &&fn, I &&init)
        : super(in_place, std::forward<Fn>(fn))
        , m_init(std::forward<I>(init)) {}

    template<class Iterable>
    constexpr auto get_init(Iterable&&) const -> auto { return m_init; }
private:
    Init m_init;
};

template<class Policy, class Fn>
struct reduce_fn<Policy, Fn, void>
        : reduce_fn_base<reduce_fn<Policy, Fn, void>, Policy, Fn> {
    using super = reduce_fn_base<reduce_fn<Policy, Fn, void>, Policy, Fn>;
    template<class F>
    constexpr reduce_fn(in_place_t, F &&fn)
        : super(in_place, std::forward<Fn>(fn)) {}
    template<class Iterable>
    auto get_init(Iterable &&it) const -> std::decay_t<decltype(it.get())>
    {
        if (it.at_end())
            return std::decay_t<decltype(it.get())>();
        auto acc = it.get();
        it.next();
        return acc;
    }
};

template<class F>
constexpr inline auto reduce(F &&fn) -> auto
{ return make_pipe(reduce_fn<run_seq_t, std::decay_t<F>, void>(in_place, std::forward<F>(fn))); }

template<class F, run_policy p>
constexpr inline auto reduce(run_policy_t<p>, F &&fn) -> auto
{ return make_pipe(reduce_fn<run_policy_t<p>, std::decay_t<F>, void>(in_place, std::forward<F>(fn))); }

template<class T, class F>
constexpr inline auto reduce(T &&init, F &&fn) -> auto
{ return make_pipe(reduce_fn<run_seq_t, std::decay_t<F>, std::decay_t<T>>(in_place, std::forward<F>(fn), std::forward<T>(init))); }

template<class T, class F, run_policy p>
constexpr inline auto reduce(run_policy_t<p>, T &&init, F &&fn) -> auto
{ return make_pipe(reduce_fn<run_policy_t<p>, std::decay_t<F>, std::decay_t<T>>(in_place, std::forward<F>(fn), std::forward<T>(init))); }

inline auto sum() -> auto
{ return reduce([] (auto &acc, auto &&v) { acc += std::forward<decltype(v)>(v); }); }

template<run_policy p>
inline auto sum(run_policy_t<p>) -> auto
{ return reduce(run_policy_t<p>(), [] (auto &acc, auto &&v) { acc += std::forward<decltype(v)>(v); }); }

}

}
