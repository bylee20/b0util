#pragma once

#include "./_range_fn.hpp"

namespace b0 { namespace range {

template<class T, class Fn, class Iterable, bool ref = false>
struct flatten_iterable : range_fn_iterable<T, Iterable>
{
    using super = range_fn_iterable<T, Iterable>;
    using sub_type = b0_autotype(*std::begin(std::declval<Iterable>()));

    template<class It>
    constexpr flatten_iterable(in_place_t, It &&it)
        : super(in_place, std::forward<It>(it))) { check(); }
    auto next() -> void
    {
        ++m_cur;
        if (m_cur == m_cur_end) {
            super::next();
            check();
        }
    }
protected:
    auto check() -> void
    {
        while (!at_end()) {
            m_range = super::get();
            m_cur = std::cbegin(m_range);
            m_cur_end = std::cend(m_range);
            if (m_cur != m_cur_end)
                break;
            super::next();
        }
    }
    begin_t<const sub_type&> m_cur;
    end_t<const sub_type&> m_cur_end;
private:
    sub_type m_range;
};

template<class T, class Fn, class Iterable>
struct flatten_iterable<T, Fn, Iterable, true> : range_fn_iterable<T, Iterable>
{
    using super = range_fn_iterable<T, Iterable>;
    using sub_type = b0_autotype(*std::begin(std::declval<Iterable>()));

    template<class It>
    constexpr flatten_iterable(in_place_t, It &&it)
        : super(in_place, std::forward<It>(it))) { check(); }
    constexpr auto get() const -> auto { return *m_cur; }
    auto next() -> void
    {
        ++m_cur;
        if (m_cur == m_cur_end) {
            super::next();
            check();
        }
    }
protected:
    auto check() -> void
    {
        while (!at_end()) {
            m_range = super::get();
            m_cur = std::cbegin(m_range);
            m_cur_end = std::cend(m_range);
            if (m_cur != m_cur_end)
                break;
            super::next();
        }
    }
    begin_t<const sub_type&> m_cur;
    end_t<const sub_type&> m_cur_end;
};


struct flatten_fn {
    template<class Iterable>
    struct iterable : range_fn_iterable<iterable<Iterable>, Iterable> {
        using super = range_fn_iterable<iterable<Iterable>, Iterable>;
        template<class It>
        range_fn_iterable(in_place_t, It &&it)
            : super(std::forward<It>(it)) {
            if (!at_end()) {
                m_cur = super::get();
            }
            check();
        }
        constexpr auto size_hint() const -> auto { return npos; }
        constexpr auto size() const = delete;
    private:
        auto check {
            while (!at_end()) {

            }
        }
        b0_autotype(std::begin(std::declval<Iterable>())) m_cur;
        b0_autotype(std::end(std::declval<Iterable>())) m_cur_end;
    };
};

}}
