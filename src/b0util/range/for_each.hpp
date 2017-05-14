#pragma once

#include "./_terminal_fn.hpp"
#include <iostream>

namespace b0 { namespace range {

template<class Fn>
struct for_each_fn : terminal_fn {
    template<class F>
    for_each_fn(in_place_t, F &&fn): m_fn(std::forward<F>(fn)) {}

    template<class Iterable>
    auto run(Iterable &&it) const -> void
    {
        for (; !it.at_end(); it.next())
            m_fn(it.get());
    }
private:
    Fn m_fn;
};

template<class Fn>
constexpr inline auto for_each(Fn &&fn) -> auto
{ return make_pipe(for_each_fn<std::decay_t<Fn>>(in_place, std::forward<Fn>(fn))); }

}}
