#pragma once

#include "./_range_fn.hpp"

namespace b0 {

namespace detail {

template<class Map, class Filter, class C>
struct map_filter_fn;

//template<class F1, class F2>
//inline auto make_map_map_filter_fn(F1 &&fn1, F2 &&fn2) -> auto
//{
//    using as1_t = typename std::decay_t<F1>::container_type;
//    using as2_t = typename std::decay_t<F2>::container_type;
//    using container = meta::if_t<!std::is_void_v<as2_t>, as2_t, as1_t>;
//    auto func = [f1=std::forward<F1>(fn1).func(), f2=std::forward<F2>(fn2).func()] (auto &&v) {
//        return f2(f1(std::forward<decltype(v)>(v)));
//    };
//    return map_filter_fn<decltype(func), container>(std::move(func), std::min(fn1.limit(), fn2.limit()));
//}

template<class Map, class Filter, class C>
struct map_filter_fn : range_as_fn<map_filter_fn<Map, Filter, C>, C, Map, Filter>
{
    using range_as_fn<map_filter_fn, C, Map, Filter>::range_as_fn;
    using range_as_fn<map_filter_fn, C, Map, Filter>::merge;

    template<class Range>
    auto operator() (Range &&range) const -> decltype(auto)
    {
        typename range_as_container<C, Range&&, const F&>::type c;
        const auto size = std::size(range);
        const auto &func = this->func();
        if (!limit().is_limited() || static_cast<int>(size) <= limit().count()) {
            try_reserve(c, std::size(range));
            const auto end = std::end(std::forward<Range>(range));
            for (auto it = std::begin(std::forward<Range>(range)); it != end; ++it)
                emplace_or_push_back(c, func(b0::forward_like<Range>(range, *it)));
            return c;
        } else {
            const int size = limit().count();
            try_reserve(c, size);
            auto it = std::begin(std::forward<Range>(range));
            for (int i = 0; i < size; ++i, ++it)
                emplace_or_push_back(c, func(b0::forward_like<Range>(range, *it)));
            return c;
        }
    }

    template<class _F, class _C>
    constexpr auto merge(map_filter_fn<_F, _C> &&other) && -> auto
    { return make_map_map_filter_fn(std::move(*this), std::move(other)); }
    template<class _F, class _C>
    constexpr auto merge(map_filter_fn<_F, _C> &&other) const& -> auto
    { return make_map_map_filter_fn(*this, std::move(other)); }
    template<class _F, class _C>
    constexpr auto merge(const map_filter_fn<_F, _C> &other) && -> auto
    { return make_map_map_filter_fn(std::move(*this), other); }
    template<class _F, class _C>
    constexpr auto merge(const map_filter_fn<_F, _C> &other) const& -> auto
    { return make_map_map_filter_fn(*this, other); }

    template<class U>
    static auto as() -> map_filter_fn<F, U>;
private:
    template<class _F, class _C>
    friend struct map_filter_fn;
};

}

template<class F>
constexpr inline auto map(F &&fn) -> auto
{ return make_range_fn_pipe(detail::map_filter_fn<std::decay_t<F>, void>(std::forward<F>(fn), range_limit())); }

}
