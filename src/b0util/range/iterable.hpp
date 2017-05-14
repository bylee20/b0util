#pragma once

#include "../meta/iterator.hpp"
#include "../meta/container.hpp"
#include "../utility.hpp"

namespace b0 { namespace range {

constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

struct iterable {};

template<class Begin, class End>
struct iterator_iterable : iterable {
    template<class B, class E>
    constexpr iterator_iterable(B &&begin, E &&end)
        : m_iterator(std::forward<B>(begin)), m_end(std::forward<E>(end)) {}

    using value_type = std::decay_t<decltype(*std::declval<Begin>())>;

    constexpr auto at_end() const -> bool { return m_iterator == m_end; }
    constexpr auto get() const -> decltype(auto) { return *m_iterator; }
    auto next() -> void { ++m_iterator; }
    constexpr auto size_hint() const -> std::size_t { return npos; }
    constexpr auto iterator() const -> Begin { return m_iterator; }
private:
    Begin m_iterator; End m_end;
};

template<class Begin, class End>
struct sized_iterator_iterable : iterator_iterable<Begin, End> {
    template<class B, class E>
    constexpr sized_iterator_iterable(B &&begin, E &&end, std::size_t size)
        : iterator_iterable<Begin, End>(std::forward<B>(begin), std::forward<E>(end)), m_size(size) {}
    constexpr auto size_hint() const -> std::size_t { return m_size; }
    constexpr auto size() const -> std::size_t { return m_size; }
private:
    std::size_t m_size;
};

template<class Range, bool = b0::meta::has_size<Range>::value>
struct range_view_iterable : iterator_iterable<
        std::decay_t<decltype(std::cbegin(std::declval<Range>()))>,
        std::decay_t<decltype(std::cend(std::declval<Range>()))>
> {
    using super = iterator_iterable<
        std::decay_t<decltype(std::cbegin(std::declval<Range>()))>,
        std::decay_t<decltype(std::cend(std::declval<Range>()))>
    >;
    template<class Rng>
    constexpr range_view_iterable(in_place_t, Rng &&rng)
        : super(std::cbegin(rng), std::cend(rng)) {}
};

template<class Range>
struct range_view_iterable<Range, true> : sized_iterator_iterable<
        std::decay_t<decltype(std::cbegin(std::declval<Range>()))>,
        std::decay_t<decltype(std::cend(std::declval<Range>()))>
> {
    using super = sized_iterator_iterable<
        std::decay_t<decltype(std::cbegin(std::declval<Range>()))>,
        std::decay_t<decltype(std::cend(std::declval<Range>()))>
    >;
    constexpr range_view_iterable(in_place_t, const Range &rng)
        : super(std::cbegin(rng), std::cend(rng), std::size(rng)) {}
};

template<class Range>
constexpr inline auto wrap(Range &&range) -> range_view_iterable<std::decay_t<Range> >
{ return {in_place, std::forward<Range>(range)}; }

}}
