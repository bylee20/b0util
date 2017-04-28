#pragma once

#include <utility>
#include "./meta.hpp"

namespace b0 { namespace meta {

template <int... Ints>
using int_seq = std::integer_sequence<int, Ints...>;

namespace detail {

template<int n, int... ints>
static constexpr auto add_to_int_seq(int_seq<ints...>)
-> auto { return int_seq<(n + ints)...>(); }

}

template<int Int, int... Ints>
constexpr auto add(int_seq<Ints...>) { return int_seq<(Int + Ints)...>(); }

template<int Int, class Seq>
using add_t = decltype(add(Seq()));

template <int N>
using make_int_seq = std::make_integer_sequence<int, N>;

template <int begin, int end>
using make_int_range_seq = decltype(detail::add_to_int_seq<begin>(make_int_seq<end - begin>()));

template <class... Args>
using int_seq_for = make_int_seq<sizeof...(Args)>;

} }
