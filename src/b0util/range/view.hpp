#pragma once

#include "../utility.hpp"
#include "../meta/container.hpp"
#include "../meta/iterator.hpp"
#include <iostream>

namespace b0 { namespace range {

template<class Iter>
struct iterator_sentinel {
    constexpr auto operator == (const Iter &it) const -> bool { return it.at_end(); }
    constexpr auto operator != (const Iter &it) const -> bool { return !it.at_end(); }
    friend constexpr auto operator == (const Iter &it, iterator_sentinel) -> bool { return it.at_end(); }
    friend constexpr auto operator != (const Iter &it, iterator_sentinel) -> bool { return !it.at_end(); }
};

template<class Range>
using iterator_t = std::decay_t<decltype(std::cbegin(std::declval<Range>()))>;
template<class Range>
using sentinel_t = std::decay_t<decltype(std::cend(std::declval<Range>()))>;

template<class Range>
using begin_t = std::decay_t<decltype(std::begin(std::declval<Range>()))>;
template<class Range>
using end_t = std::decay_t<decltype(std::end(std::declval<Range>()))>;

template<class Begin, class End>
struct iterator_range {
    template<class B, class E>
    constexpr iterator_range(B &&begin, E &&end)
        : m_begin(std::forward<B>(begin)), m_end(std::forward<E>(end)) {}

    using value_type = std::decay_t<decltype(*std::declval<Begin>())>;

    constexpr auto begin() const -> Begin { return m_begin; }
    constexpr auto end() const -> End { return m_end; }
private:
    Begin m_begin; End m_end;
};

template<class Begin, class End, class S>
struct sized_iterator_range : iterator_range<Begin, End> {
    template<class B, class E>
    constexpr sized_iterator_range(B &&begin, E &&end, S size)
        : iterator_range<Begin, End>(std::forward<B>(begin), std::forward<E>(end))
        , m_size(size) {}
    constexpr auto size() const -> S { return m_size; }
private:
    S m_size;
};

namespace detail {
template<class Range>
constexpr inline auto make_view(Range &&range, meta::true_)
-> sized_iterator_range<begin_t<Range&>, end_t<Range&>, std::decay_t<decltype(std::size(range))> >
{ return {std::begin(range), std::end(range), std::size(range)}; }

template<class Range>
constexpr inline auto make_view(Range &&range, meta::false_)
-> iterator_range<begin_t<Range&>, end_t<Range&> >
{ return {std::begin(range), std::end(range)}; }

template<class Begin, class End>
constexpr inline auto make_iterator_view(Begin &&begin, End &&end, meta::true_)
-> sized_iterator_range<std::decay_t<Begin>, std::decay_t<End>, std::decay_t<decltype(std::declval<End>() - std::declval<Begin>())> >
{ return {std::forward<Begin>(begin), std::forward<End>(end), end - begin}; }

template<class Begin, class End>
constexpr inline auto make_iterator_view(Begin &&begin, End &&end, meta::false_)
-> iterator_range<std::decay_t<Begin>, std::decay_t<End> >
{ return {std::forward<Begin>(begin), std::forward<End>(end)}; }
}

template<class Begin, class End>
constexpr inline auto view(Begin &&begin, End &&end) -> auto
{ return detail::make_iterator_view(std::forward<Begin>(begin), std::forward<End>(end), b0::meta::is_random_access_iterator<std::decay_t<Begin>>()); }

template<class Begin, class End, class S>
constexpr inline auto view(Begin &&begin, End &&end, S size)
-> sized_iterator_range<std::decay_t<Begin>, std::decay_t<End>, S>
{ return { std::forward<Begin>(begin), std::forward<End>(end), size }; }

template<class Begin, class D>
constexpr inline auto view_n(Begin &&begin, D diff)
-> sized_iterator_range<std::decay_t<Begin>, std::decay_t<decltype(begin + diff)>, D>
{ return { begin, begin + diff, diff }; }

template<class Range>
constexpr inline auto cview(const Range &range) -> auto
{ return detail::make_view(range, meta::bool_<meta::has_size_v<Range>>()); }

template<class Range>
constexpr inline auto view(Range &range) -> auto
{ return detail::make_view(std::forward<Range>(range), meta::bool_<meta::has_size_v<Range>>()); }

}}
