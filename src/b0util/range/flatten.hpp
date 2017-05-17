#pragma once

#include "./_range_fn.hpp"

namespace b0 { namespace range {

template<class T, class Iterable, bool ref = false>
struct flatten_iterable : range_fn_iterable<T, Iterable>
{
    using super = range_fn_iterable<T, Iterable>;
    using range_type = b0_autotype(std::declval<super>().super::get());
    using value_type = b0_autotype(*std::begin(std::declval<range_type>()));

    template<class It>
    constexpr flatten_iterable(in_place_t, It &&it)
        : super(in_place, std::forward<It>(it)) { }
protected:
    auto check() -> void
    {
        while (!super::at_end()) {
            m_range = super::get();
            m_cur = std::cbegin(m_range);
            m_cur_end = std::cend(m_range);
            if (m_cur != m_cur_end)
                break;
            super::next();
        }
    }
    begin_t<const range_type&> m_cur;
    end_t<const range_type&> m_cur_end;
private:
    range_type m_range;
};

template<class T, class Iterable>
struct flatten_iterable<T, Iterable, true> : range_fn_iterable<T, Iterable>
{
    using super = range_fn_iterable<T, Iterable>;
    using range_type = b0_autotype(std::declval<super>().super::get());
    using value_type = b0_autotype(*std::begin(std::declval<range_type>()));

    template<class It>
    constexpr flatten_iterable(in_place_t, It &&it)
        : super(in_place, std::forward<It>(it)) { }
protected:
    auto check() -> void
    {
        while (!super::at_end()) {
            m_cur = std::cbegin(super::get());
            m_cur_end = std::cend(super::get());
            if (m_cur != m_cur_end)
                break;
            super::next();
        }
    }
    begin_t<const range_type&> m_cur;
    end_t<const range_type&> m_cur_end;
};


struct flatten_fn {
    template<class Iterable>
    struct iterable : flatten_iterable<
            iterable<Iterable>, Iterable,
            std::is_reference_v<decltype(std::declval<Iterable>().get())>
    > {
        using super = flatten_iterable<iterable<Iterable>, Iterable,
            std::is_reference_v<decltype(std::declval<Iterable>().get())> >;

        template<class It>
        iterable(in_place_t, It &&it)
            : super(in_place, std::forward<It>(it)) { super::check(); }
        constexpr auto get() const -> auto { return *super::m_cur; }
        constexpr auto size_hint() const -> auto { return npos; }
        constexpr auto size() const = delete;
        auto next() -> void
        {
            ++super::m_cur;
            if (super::m_cur == super::m_cur_end) {
                super::next();
                super::check();
            }
        }
    };

    template<class Iterable>
    constexpr auto make_iterable(Iterable &&it) const& -> iterable<std::decay_t<Iterable> >
    { return { in_place, std::forward<Iterable>(it) }; }

    template<class Iterable>
    constexpr auto make_iterable(Iterable &&it) && -> iterable<std::decay_t<Iterable> >
    { return { in_place, std::forward<Iterable>(it) }; }
};

constexpr inline auto flatten() -> auto { return make_pipe(flatten_fn()); }

}}
