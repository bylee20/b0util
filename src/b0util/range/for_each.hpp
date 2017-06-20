#pragma once

#include "./_terminal_fn.hpp"
#include <iostream>

namespace b0 { namespace range {

template<class Policy, class Fn>
struct for_each_fn : terminal_fn {
    using run_policy = Policy;

    template<class F>
    for_each_fn(in_place_t, F &&fn): m_fn(std::forward<F>(fn)) {}

    template<class Iterable>
    auto run(Iterable &&it) const -> void
    {
        for (; !it.at_end(); it.next())
            b0::invoke(m_fn, it.get());
    }
private:
    Fn m_fn;
};

template<class Fn, run_policy p>
constexpr inline auto for_each(run_policy_t<p>, Fn &&fn) -> auto
{ return make_pipe(for_each_fn<run_policy_t<p>, std::decay_t<Fn>>(in_place, std::forward<Fn>(fn))); }

template<class Fn>
constexpr inline auto for_each(Fn &&fn) -> auto
{ return for_each(run_seq, std::forward<Fn>(fn)); }

}}
