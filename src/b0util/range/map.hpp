#pragma once

#include "./_range_fn.hpp"
#include "../meta/callable.hpp"

namespace b0 {

namespace range {

template<class Fn>
struct map_fn {
    template<class Iterable>
    struct iterable : range_fn_iterable<iterable<Iterable>, Iterable> {
        using super = range_fn_iterable<iterable<Iterable>, Iterable>;
        template<class F, class It>
        constexpr iterable(It &&it, F &&fn)
            : super(in_place, std::forward<It>(it))
            , m_fn(std::forward<F>(fn)) {}
        using value_type = meta::invoke_t<Fn, const typename super::value_type&>;
#ifdef B0_CC_MSVC
#pragma warning(push)
#pragma warning(disable: 4100)
#endif
        constexpr auto get() const -> decltype(auto) { return b0::invoke(m_fn, super::get()); }
#ifdef B0_CC_MSVC
#pragma warning(pop)
#endif
    private:
        Fn m_fn;
    };

    template<class Fn>
    constexpr map_fn(in_place_t, Fn &&fn)
        : m_fn(std::forward<Fn>(fn)) {}

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
constexpr inline auto map(F &&f) -> auto
{ return make_pipe(map_fn<std::decay_t<F>>(in_place, std::forward<F>(f))); }

}
}
