#pragma once

#include "./pipe.hpp"
#include "./view.hpp"
#include "./iterable.hpp"
#include "../container.hpp"
#include <vector>
#include <iterator>

namespace b0 { namespace range {

template<class T>
struct range_fn {
    template<class Range>
    auto apply(Range &&range) -> auto
    {
        using iterable = typename T::template iterable<std::decay_t<Range>>;
        iterable it(static_cast<const T*>(this), std::forward<Range>(range));
        using value_t = typename iterable::value_type;
        std::vector<value_t> c;
        for (; !it->at_end(); it.next())
            c.emplace_back(it.get());
        return c;
    }
};

template<class T, class Iterable>
struct range_fn_iterable_base : b0::range::iterable {
    template<class It>
    range_fn_iterable_base(in_place_t, It &&it)
        : m_it(std::forward<It>(it)) {}
    using value_type = typename Iterable::value_type;
    constexpr auto at_end() const -> bool { return m_it.at_end(); }
    constexpr auto get() const -> decltype(auto) { return m_it.get(); }
    auto next() -> void { m_it.next(); }
    constexpr auto size_hint() const -> auto { return m_it.size_hint(); }

    auto run() && -> auto
    {
        auto &&it = static_cast<T&&>(*this);
        std::vector<typename T::value_type> c;
        const auto hint = it.size_hint();
        if (hint != npos)
            try_reserve(c, hint);
        for (; !it.at_end(); it.next())
            emplace_or_push_back(c, it.get());
        return c;
    }
    constexpr auto iterator() const
    -> decltype(std::declval<Iterable>().iterator()) { return m_it.iterator(); }
protected:
    constexpr auto _iterable() const -> const Iterable& { return m_it; }
private:
    Iterable m_it;
};


template<class T, class Iterable, bool = meta::has_size<Iterable>::value>
struct range_fn_iterable;

template<class T, class Iterable>
struct range_fn_iterable<T, Iterable, false> : range_fn_iterable_base<T, Iterable> {
    template<class It>
    range_fn_iterable(in_place_t, It &&it)
        : range_fn_iterable_base<T, Iterable>(in_place, std::forward<It>(it)) {}
};

template<class T, class Iterable>
struct range_fn_iterable<T, Iterable, true> : range_fn_iterable_base<T, Iterable> {
    template<class It>
    range_fn_iterable(in_place_t, It &&it)
        : range_fn_iterable_base<T, Iterable>(in_place, std::forward<It>(it)) {}
    constexpr auto size() const -> decltype(auto) { return std::size(this->_iterable()); }
};

}}
