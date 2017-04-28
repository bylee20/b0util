#pragma once

#include "./iterable.hpp"
#include "./_terminal_fn.hpp"
#include "../container.hpp"

namespace b0 { namespace range {

template<class Policy, class C>
struct to_container_fn : terminal_fn {
    using run_policy = Policy;

    template<class Iterable>
    static auto run(Iterable &&it) -> C
    {
        using namespace meta;
        C cont;
        if (it.size_hint() != npos)
            try_reserve(cont, it.size_hint());
        for (; !it.at_end(); it.next())
            emplace_or_push_back(cont, it.get());
        return cont;
    }

    static auto parallel(std::vector<C> &&ranges) -> C
    {
        if (ranges.empty())
            return {};
        std::size_t size = 0;
        for (const auto &r : ranges)
            size += static_cast<std::size_t>(std::size(r));
        C cont;
        cont.reserve(size);
        auto inserter = std::back_inserter(cont);
        for (auto &r : ranges)
            std::move(r.begin(), r.end(), inserter);
        return cont;
    }
};

template<class Policy, template<class...> class C>
struct to_container_generic_fn : terminal_fn {
    using run_policy = Policy;

    template<class Iterable, class  T = typename std::decay_t<Iterable>::value_type>
    static inline auto run(Iterable &&it) -> C<T>
    { return to_container_fn<Policy, C<T>>::run(std::forward<Iterable>(it)); }

    template<class T>
    static auto parallel(std::vector<T> &&ranges) -> T
    { return to_container_fn<Policy, T>::parallel(std::move(ranges)); }
};

template<template<class...> class C>
constexpr inline auto to() { return make_pipe(to_container_generic_fn<run_seq_t, C>()); }

template<class C>
constexpr inline auto to() { return make_pipe(to_container_fn<run_seq_t, C>()); }

template<template<class...> class C, run_policy p>
constexpr inline auto to(run_policy_t<p>) { return make_pipe(to_container_generic_fn<run_policy_t<p>, C>()); }

template<class C, run_policy p>
constexpr inline auto to(run_policy_t<p>) { return make_pipe(to_container_fn<run_policy_t<p>, C>()); }

}}
