#pragma once

#include "./_range_fn.hpp"

namespace b0 { namespace range {

struct limit_fn {
    constexpr limit_fn(int count): m_count(count) {}

    template<class Iterable>
    struct iterable : range_fn_iterable<iterable<Iterable>, Iterable> {
        using super = range_fn_iterable<iterable<Iterable>, Iterable>;
        template<class It>
        iterable(It &&it, int count)
            : super(in_place, std::forward<It>(it)), m_count(count) {}
        auto next() -> void { ++m_counter; super::next(); }
        auto size_hint() const -> std::size_t { return std::min<std::size_t>(super::size_hint(), m_count); }
        constexpr auto at_end() const -> bool
            { return m_counter >= m_count || super::at_end(); }
    private:
        int m_count, m_counter = 0;
    };

    template<class Range>
    constexpr auto make_iterable(Range &&range) const -> auto
    { return iterable<std::decay_t<Range>>(std::forward<Range>(range), m_count); }
private:
    int m_count = 0;
};

constexpr inline auto limit(int count) -> auto
{ return make_pipe(limit_fn(count)); }

}}

