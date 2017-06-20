#pragma once

#include "./range/map.hpp"
#include "./range/filter.hpp"
#include "./range/limit.hpp"
#include "./range/counter.hpp"
#include "./range/reduce.hpp"
#include "./range/count.hpp"
#include "./range/copy.hpp"
#include "./range/find.hpp"
#include "./range/flatten.hpp"
#include "./range/for_each.hpp"
#include "./range/to.hpp"
#include "./range/view.hpp"

namespace b0 { namespace range {

template<class T>
constexpr inline auto repeat(T &&value) -> auto
{ return counter(0) | map([t=std::forward<T>(value)] (auto) { return t; }); }

template<class T, class S>
constexpr inline auto repeat(T &&value, S count) -> auto
{ return repeat(std::forward<T>(value)) | limit(static_cast<int>(count)); }

template<template<class...> class Cont, class Range, class Fn>
constexpr inline auto map_to(Range &&range, Fn &&fn) -> auto
{ return std::forward<Range>(range) | map(std::forward<Fn>(fn)) | to<Cont>(); }

template<class Cont, class Range, class Fn>
constexpr inline auto map_to(Range &&range, Fn &&fn) -> auto
{ return std::forward<Range>(range) | map(std::forward<Fn>(fn)) | to<Cont>(); }

template<template<class...> class Cont, class Range, class Fn, run_policy p>
constexpr inline auto map_to(run_policy_t<p>, Range &&range, Fn &&fn) -> auto
{ return std::forward<Range>(range) | map(std::forward<Fn>(fn)) | to<Cont>(run_policy_t<p>()); }

template<class Cont, class Range, class Fn, run_policy p>
constexpr inline auto map_to(run_policy_t<p>, Range &&range, Fn &&fn) -> auto
{ return std::forward<Range>(range) | map(std::forward<Fn>(fn)) | to<Cont>(run_policy_t<p>()); }

template<class Range, class Fn>
constexpr inline auto for_each(Range &&range, Fn &&fn) -> auto
{ return std::forward<Range>(range) | for_each(std::forward<Fn>(fn)); }

template<class Range, class Fn, run_policy p>
constexpr inline auto for_each(run_policy_t<p>, Range &&range, Fn &&fn) -> auto
{ return std::forward<Range>(range) | for_each(run_policy_t<p>(), std::forward<Fn>(fn)); }

}}
