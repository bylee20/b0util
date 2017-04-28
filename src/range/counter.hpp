#pragma once

#include "./_range_fn.hpp"

namespace b0 { namespace range {

template<class T, class Range>
struct counter_iterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using difference_type = b0_autotype(std::declval<T>() - std::declval<T>());
    using pointer = T*;
    using reference = T&;

    template<class U>
    constexpr counter_iterator(const Range *range, U &&value)
        : m_range(range), m_value(std::forward<U>(value)) {}
    constexpr auto operator * () const -> const T& { return m_value; }
    constexpr auto at_end() const -> bool { return false; }
    constexpr auto operator == (const counter_iterator &rhs) const -> bool
    { return m_range == rhs.m_range && m_value == rhs.m_value; }
protected:
    const Range *m_range = nullptr;
    T m_value;
};

template<class T, class D>
struct counter_range {
    struct iterator : counter_iterator<T, counter_range> {
        template<class U>
        constexpr iterator(const counter_range *range, U &&value)
            : counter_iterator<T, counter_range>(range, std::forward<U>(value)) {}
        auto operator ++() -> auto& { this->m_value += this->m_range->m_step; return *this; }
    };
    template<class U, class V>
    constexpr counter_range(in_place_t, U &&begin, V &&step)
        : m_begin(std::forward<U>(begin)), m_step(std::forward<V>(step)) {}

    constexpr auto begin() const -> iterator { return iterator(this, m_begin); }
    constexpr auto end() const -> iterator_sentinel<iterator> { return {}; }
private:
    T m_begin; D m_step;
};

template<class T>
struct counter_range<T, void> {
    struct iterator : counter_iterator<T, counter_range> {
        template<class U>
        constexpr iterator(const counter_range *range, U &&value)
            : counter_iterator<T, counter_range>(range, std::forward<U>(value)) {}
        auto operator ++() -> auto& { ++this->m_value; return *this; }
    };
    using sentinel = iterator_sentinel<iterator>;
    template<class U>
    constexpr counter_range(in_place_t, U &&begin)
        : m_begin(std::forward<U>(begin)) {}

    constexpr auto begin() const -> iterator { return iterator(this, m_begin); }
    constexpr auto end() const -> iterator_sentinel<iterator> { return {}; }
private:
    T m_begin;
};

template<class T>
constexpr inline auto counter(T &&begin) -> auto
{ return counter_range<std::decay_t<T>, void>(in_place, std::forward<T>(begin)); }

template<class T, class D>
constexpr inline auto counter(T &&begin, D &&step) -> auto
{ return counter_range<std::decay_t<T>, std::decay_t<D>>(in_place, std::forward<T>(begin), std::forward<D>(step)); }

}}
