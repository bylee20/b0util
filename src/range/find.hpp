#pragma once

#include "./_terminal_fn.hpp"

namespace b0 { namespace range {

template<class Fn>
struct find_if_fn : terminal_fn {
    template<class F>
    find_if_fn(in_place_t, F &&fn): m_fn(std::forward<F>(fn)) {}

    template<class Iterable>
    auto run(Iterable &&it) const -> auto
    {
        for (; !it.at_end(); it.next()) {
            if (m_fn(it.get()))
                break;
        }
        return it.iterator();
    }
private:
    Fn m_fn;
};

template<class Fn>
constexpr inline auto find_if(Fn &&fn) -> auto
{ return make_pipe(find_if_fn<std::decay_t<Fn>>(in_place, std::forward<Fn>(fn))); }

template<class T>
constexpr inline auto find(T &&t) -> auto
{ return find_if([t=std::forward<T>(t)] (const auto &v) -> bool { return t == v; }); }

}}
