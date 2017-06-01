#pragma once

#include "./meta/meta.hpp"
#include <utility>

namespace b0 {

struct in_place_t {
    explicit constexpr in_place_t() = default;
};

constexpr in_place_t in_place{};

template<class T>
struct in_place_type_t {
    explicit constexpr in_place_type_t() = default;
};

template <class T>
constexpr in_place_type_t<T> in_place_type{};

template <int I>
struct in_place_index_t {
    explicit constexpr in_place_index_t() = default;
};

template <int I>
constexpr in_place_index_t<I> in_place_index{};

template<class T>
inline constexpr auto const_(const T &t) -> const auto& { return t; }

struct uncopyable {
    constexpr uncopyable() = default;
    uncopyable(const uncopyable&) = delete;
    auto operator = (const uncopyable&) -> uncopyable& = delete;
};

template<class T, class U>
inline constexpr auto forward_like(meta::unref_t<T>&, U &u)
-> meta::req_t<meta::is_rref_v<T&&>, U&&> { return std::move(u); }
template<class T, class U>
inline constexpr auto forward_like(meta::unref_t<T>&, U &u)
-> meta::req_t<meta::is_lref_v<T&&>, U&> { return u; }

enum class thread_safety { unsafe, safe };

}
