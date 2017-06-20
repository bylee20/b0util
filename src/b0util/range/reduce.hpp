#pragma once

#include "./_terminal_fn.hpp"

namespace b0 {

namespace range {

template<class T, class Policy, class Fn>
struct reduce_fn_base : terminal_fn {
    using run_policy = Policy;

    template<class F>
    constexpr reduce_fn_base(in_place_t, F &&fn)
        : m_fn(std::forward<F>(fn)) {}
    template<class Iterable>
    auto run(Iterable &&it) const -> auto
    {
        auto acc = static_cast<const T&>(*this).get_init(it);
        for (; !it.at_end(); it.next())
            b0::invoke(m_fn, acc, it.get());
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
            b0::invoke(m_fn, init, std::move(*it));
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
        : super(in_place, std::forward<F>(fn))
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
        : super(in_place, std::forward<F>(fn)) {}
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
constexpr inline auto sum(run_policy_t<p>) -> auto
{ return reduce(run_policy_t<p>(), [] (auto &acc, auto &&v) { acc += std::forward<decltype(v)>(v); }); }

template<class LessThan>
inline auto max(LessThan &&lt) -> auto
{
    return reduce([lt=std::forward<LessThan>(lt)] (auto &acc, auto &&v) {
        if (lt(acc, v))
            acc = std::forward<decltype(v)>(v);
    });
}

inline auto max() -> auto { return max(std::less<>()); }

template<class LessThan>
inline auto min(LessThan &&lt) -> auto
{
    return reduce([lt=std::forward<LessThan>(lt)] (auto &acc, auto &&v) {
        if (lt(v, acc))
            acc = std::forward<decltype(v)>(v);
    });
}

inline auto min() -> auto { return min(std::less<>()); }

template<class Policy, class LessThan>
struct minmax_fn : terminal_fn {
    using run_policy = Policy;

    template<class F>
    constexpr minmax_fn(in_place_t, F &&fn)
        : m_lt(std::forward<F>(fn)) {}
    template<class Iterable>
    auto run(Iterable &&it) const -> auto
    {
        using T = typename std::decay_t<Iterable>::value_type;
        if (it.at_end())
            return std::pair<T, T>();
        auto tmp = it.get();
        std::pair<T, T> pair(tmp, tmp);
        for (it.next(); !it.at_end(); it.next()) {
            auto tmp = it.get();
            if (m_lt(tmp, pair.first))
                pair.first = tmp;
            else if (m_lt(pair.second, tmp))
                pair.second = tmp;
        }
        return pair;
    }
    template<class T>
    auto parallel(std::vector<std::pair<T, T>> &&values) const -> std::pair<T, T>
    {
        if (values.empty())
            return {};
        auto it = values.begin();
        auto pair = std::move(*it);
        for (++it; it != values.end(); ++it) {
            if (m_lt(it->first, pair.first))
                pair.first = it->first;
            else if (m_lt(pair.second, it->second))
                pair.second = it->second;
        }
        return pair;
    }
protected:
    LessThan m_lt;
};

template<class LessThan>
constexpr inline auto minmax(LessThan &&lt) -> auto
{ return make_pipe(minmax_fn<run_seq_t, std::decay_t<LessThan>>(in_place, std::forward<LessThan>(lt))); }

template<class LessThan, run_policy p>
constexpr inline auto minmax(run_policy_t<p>, LessThan &&fn) -> auto
{ return make_pipe(minmax_fn<run_policy_t<p>, std::decay_t<LessThan>>(in_place, std::forward<LessThan>(fn))); }

constexpr inline auto minmax() -> auto { return minmax(std::less<>()); }

template<run_policy p>
constexpr inline auto minmax(run_policy_t<p>) -> auto { return minmax(run_policy_t<p>(), std::less<>()); }

}

}
