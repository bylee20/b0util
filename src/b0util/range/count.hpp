#pragma once

#include "./_terminal_fn.hpp"

namespace b0 {

namespace range {

template<class Fn>
struct count_if_fn : terminal_fn {
    template<class F>
    constexpr count_if_fn(in_place_t, F &&fn)
        : m_fn(std::forward<F>(fn)) {}
    template<class Iterable>
    auto run(Iterable &&it) const -> int
    {
        int count = 0;
        for (; !it.at_end(); it.next()) {
            if (m_fn(it.get()))
                ++count;
        }
        return count;
    }
private:
    Fn m_fn;
};

struct count_fn : terminal_fn {
    constexpr count_fn() = default;
    template<class Iterable>
    constexpr auto run(Iterable &&it) const -> auto
        { return run(std::forward<Iterable>(it), meta::has_size<Iterable>()); }
private:
    template<class Iterable>
    constexpr auto run(Iterable &&it, meta::true_) const -> auto
        { return std::size(it); }
    template<class Iterable>
    auto run(Iterable &&it, meta::false_) const -> int
    {
        int count = 0;
        for (; !it.at_end(); it.next()) { ++count; }
        return count;
    }
};

template<class F>
constexpr inline auto count_if(F &&f) -> auto
{ return make_pipe(count_if_fn<std::decay_t<F>>(in_place, std::forward<F>(f))); }

constexpr inline auto count() -> auto
{ return make_pipe(count_fn()); }

}

}
