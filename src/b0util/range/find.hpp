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
            if (b0::invoke(m_fn, it.get()))
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

template<class Fn>
struct find_max_fn : terminal_fn {
    template<class F>
    constexpr find_max_fn(in_place_t, F &&fn): m_fn(std::forward<F>(fn)) {}

    template<class Iterable>
    auto run(Iterable &&it) const -> auto
    {
        auto pos = it.iterator();
        if (!it.at_end()) {
            auto value = it.get();
            for (it.next(); !it.at_end(); it.next()) {
                auto tmp = it.get();
                if (b0::invoke(m_fn, value, tmp)) {
                    value = std::move(tmp);
                    pos = it.iterator();
                }
            }
        }
        return pos;
    }
private:
    Fn m_fn;
};

template<class LessThan>
constexpr inline auto find_max(LessThan &&lt) -> auto
{ return make_pipe(find_max_fn<std::decay_t<LessThan>>(in_place, std::forward<LessThan>(lt))); }

template<class Fn>
struct find_min_fn : terminal_fn {
    template<class F>
    constexpr find_min_fn(in_place_t, F &&fn): m_fn(std::forward<F>(fn)) {}

    template<class Iterable>
    auto run(Iterable &&it) const -> auto
    {
        auto pos = it.iterator();
        if (!it.at_end()) {
            auto value = it.get();
            for (it.next(); !it.at_end(); it.next()) {
                auto tmp = it.get();
                if (b0::invoke(m_fn, tmp, value)) {
                    value = std::move(tmp);
                    pos = it.iterator();
                }
            }
        }
        return pos;
    }
private:
    Fn m_fn;
};

template<class LessThan>
constexpr inline auto find_min(LessThan &&lt) -> auto
{ return make_pipe(find_min_fn<std::decay_t<LessThan>>(in_place, std::forward<LessThan>(lt))); }

constexpr inline auto find_max() -> auto { return find_max(std::less<>()); }
constexpr inline auto find_min() -> auto { return find_min(std::less<>()); }

template<class Fn>
struct find_minmax_fn : terminal_fn {
    template<class F>
    constexpr find_minmax_fn(in_place_t, F &&fn): m_lt(std::forward<F>(fn)) {}

    template<class Iterable>
    auto run(Iterable &&it) const -> auto
    {
        auto pos = it.iterator();
        auto pair = std::make_pair(pos, pos);
        if (!it.at_end()) {
            auto min = it.get(), max = min;
            for (it.next(); !it.at_end(); it.next()) {
                auto tmp = it.get();
                if (m_lt(tmp, min)) {
                    min = std::move(tmp);
                    pair.first = it.iterator();
                } else if (m_lt(max, tmp)) {
                    max = std::move(tmp);
                    pair.second = it.iterator();
                }
            }
        }
        return pair;
    }
private:
    Fn m_lt;
};

template<class LessThan>
constexpr inline auto find_minmax(LessThan &&lt) -> auto
{ return make_pipe(find_minmax_fn<std::decay_t<LessThan>>(in_place, std::forward<LessThan>(lt))); }

constexpr inline auto find_minmax() -> auto { return find_minmax(std::less<>()); }

}}
