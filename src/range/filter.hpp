#pragma once

#include "./_range_fn.hpp"
#include "../meta/callable.hpp"
#include <iostream>

namespace b0 {

namespace range {

template<class T, class Fn, class Iterable, bool ref = false>
struct filter_iterable : range_fn_iterable<T, Iterable>
{
    using super = range_fn_iterable<T, Iterable>;
    template<class It, class F>
    constexpr filter_iterable(It &&it, F &&f)
        : super(in_place, std::forward<It>(it)), m_fn(std::forward<F>(f)) {}
    constexpr auto get() const -> typename super::value_type { return m_cache; }
protected:
    auto check() -> void
    {
        while (!at_end()) {
            m_cache = super::get();
            if (m_fn(m_cache))
                break;
            super::next();
        }
    }
    typename super::value_type m_cache{};
    Fn m_fn;
};

template<class T, class Fn, class Iterable>
struct filter_iterable<T, Fn, Iterable, true> : range_fn_iterable<T, Iterable>
{
    using super = range_fn_iterable<T, Iterable>;
    template<class It, class F>
    constexpr filter_iterable(It &&it, F &&f)
        : super(in_place, std::forward<It>(it)), m_fn(std::forward<F>(f)) {}
protected:
    auto check() -> void
    {
        while (!at_end()) {
            if (m_fn(get()))
                break;
            super::next();
        }
    }
    Fn m_fn;
};

template<class Fn>
struct filter_fn {
    template<class Iterable>
    struct iterable : filter_iterable<iterable<Iterable>, Fn, Iterable,
            std::is_reference_v<decltype(std::declval<Iterable>().get())> >
    {
        using super = filter_iterable<iterable<Iterable>, Fn, Iterable,
            std::is_reference_v<decltype(std::declval<Iterable>().get())> >;
        template<class It, class F>
        constexpr iterable(It &&it, F &&f)
            : super(std::forward<It>(it), std::forward<F>(f)) { check(); }
        auto next() -> void { super::next(); check(); }
    };

    template<class F>
    constexpr filter_fn(in_place_t, F &&fn): m_fn(std::forward<F>(fn)) {}

    template<class Iterable>
    constexpr auto make_iterable(Iterable &&it) const& -> iterable<std::decay_t<Iterable> >
    { return { std::forward<Iterable>(it), m_fn }; }

    template<class Iterable>
    constexpr auto make_iterable(Iterable &&it) && -> iterable<std::decay_t<Iterable> >
    { return { std::forward<Iterable>(it), std::move(m_fn) }; }
private:
    Fn m_fn;
};

template<class F>
constexpr inline auto filter(F &&f) -> auto
{ return make_pipe(filter_fn<std::decay_t<F>>(in_place, std::forward<F>(f))); }

}}
