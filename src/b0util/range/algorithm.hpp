#pragma once

#include "./map.hpp"
#include "./filter.hpp"
#include "./limit.hpp"
#include "./counter.hpp"
#include "./reduce.hpp"
#include "./count.hpp"
#include "./copy.hpp"
#include "./find.hpp"
#include "./flatten.hpp"
#include "./for_each.hpp"
#include "./to.hpp"
#include "./view.hpp"

namespace b0 { namespace range {

template<class T>
constexpr inline auto repeat(T &&value) -> auto
{ return counter(0) | map([t=std::forward<T>(value)] (auto) { return t; }); }

template<class T, class S>
constexpr inline auto repeat(T &&value, S count) -> auto
{ return repeat(std::forward<T>(value)) | limit(static_cast<int>(count)); }

}}
