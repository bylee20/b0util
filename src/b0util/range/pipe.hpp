#pragma once

#include "../async.hpp"
#include "./view.hpp"
#include "./iterable.hpp"
#include "../meta/int_seq.hpp"
#include <cassert>
#include <vector>

namespace b0 {

namespace range { struct terminal_fn; }

namespace meta {

template<class T>
using is_terminal = std::is_base_of<b0::range::terminal_fn, T>;

}

namespace range {

template<class Range>
struct pipe_prepend {
    template<class Pipe>
    static constexpr auto apply(Range &&range, Pipe &&pipe) -> auto
    { return make_range_pipe(std::forward<Range>(range), std::forward<Pipe>(pipe)); }

    template<class Pipe>
    static constexpr auto apply(const Range &range, Pipe &&pipe) -> auto
    { return make_range_pipe(view(range), std::forward<Pipe>(pipe)); }

    template<class Pipe>
    static constexpr auto apply(Range &range, Pipe &&pipe) -> auto
    { return make_range_pipe(view(range), std::forward<Pipe>(pipe)); }
};

template<class T, class FnTuple, bool terminated>
struct pipe_fn_base {
    META_ASSERT(meta::is_tuple_v<FnTuple>);
    template<class Fn>
    constexpr pipe_fn_base(in_place_t, Fn &&fn): m_fnTuple(std::forward<Fn>(fn)) {}
    pipe_fn_base(T &&t): m_fnTuple(std::move(t.tuple())) {}
    pipe_fn_base(const T &t): m_fnTuple(t.tuple()) {}

    using tuple_type = FnTuple;
    static constexpr int size_v = std::tuple_size_v<FnTuple>;
    static constexpr bool is_terminated = terminated;

    template<class P>
    using concat_tuple_t = decltype(
        std::tuple_cat(std::declval<FnTuple>(), std::declval<typename P::tuple_type>()));

    static constexpr auto size() -> int { return std::tuple_size_v<FnTuple>; }

    template<class Range, B0_REQ(meta::is_range<Range>::value)>
    friend constexpr auto operator | (Range &&range, const pipe_fn_base &pipe) -> auto
    { return pipe_prepend<std::decay_t<Range>>::apply(std::forward<Range>(range), static_cast<const T&>(pipe)); }

    template<class Range, B0_REQ(meta::is_range<Range>::value)>
    friend constexpr auto operator | (Range &&range, pipe_fn_base &&pipe) -> auto
    { return pipe_prepend<std::decay_t<Range>>::apply(std::forward<Range>(range), static_cast<T&&>(pipe)); }

    constexpr auto tuple() const& -> const FnTuple& { return m_fnTuple; }
    constexpr auto tuple() && -> FnTuple&& { return std::move(m_fnTuple); }
private:
    FnTuple m_fnTuple;
};

template<class Tuple, bool t>
struct pipe_fn;

template<class FnTuple>
struct pipe_fn<FnTuple, false> : pipe_fn_base<pipe_fn<FnTuple, false>, FnTuple, false> {
    using super = pipe_fn_base<pipe_fn<FnTuple, false>, FnTuple, false>;
    template<class Fn>
    constexpr pipe_fn(in_place_t, Fn &&fn): super(in_place, std::forward<Fn>(fn)) {}

//#pragma warning(push)
//#pragma warning(disable: 4100)
//    template<class Range>
//    constexpr  auto operator () (Range &&range) const -> auto
//    { return this->make_range_iterable(std::forward<Range>(range)).run(); }
//#pragma warning(pop)

    template<class Tuple, bool t>
    constexpr auto operator | (const pipe_fn<Tuple, t> &rhs) const&
    -> pipe_fn<meta::tuple_cat_t<FnTuple, Tuple>, t>
    { return { in_place, std::tuple_cat(this->tuple(), rhs.tuple()) }; }

    template<class Tuple, bool t>
    constexpr auto operator | (const pipe_fn<Tuple, t> &rhs) &&
    -> pipe_fn<meta::tuple_cat_t<FnTuple, Tuple>, t>
    { return { in_place, std::tuple_cat(this->tuple(), rhs.tuple()) }; }

    template<class Tuple, bool t>
    constexpr auto operator | (pipe_fn<Tuple, t> &&rhs) const&
    -> pipe_fn<meta::tuple_cat_t<FnTuple, Tuple>, t>
    { return { in_place, std::tuple_cat(this->tuple(), std::move(rhs.tuple())) }; }

    template<class Tuple, bool t>
    constexpr auto operator | (pipe_fn<Tuple, t> &&rhs) &&
    -> pipe_fn<meta::tuple_cat_t<FnTuple, Tuple>, t>
    { return { in_place, std::tuple_cat(this->tuple(), std::move(rhs.tuple())) }; }
};

template<class FnTuple>
struct pipe_fn<FnTuple, true> : pipe_fn_base<pipe_fn<FnTuple, true>, FnTuple, true> {
    using super = pipe_fn_base<pipe_fn<FnTuple, true>, FnTuple, true>;
    template<class Fn>
    constexpr pipe_fn(in_place_t, Fn &&fn): super(in_place, std::forward<Fn>(fn)) {}

    static constexpr int i_back = super::size_v - 1;

#pragma warning(push)
#pragma warning(disable: 4100)

    template<class Range>
    auto apply(Range &&range, run_seq_t) const -> auto
    { return get<i_back>().run(this->make_range_iterable(std::forward<Range>(range))); }

    template<class Range>
    auto apply(Range &&range, run_par_async_t) const& -> auto
    {
        return b0::async(run_par_async, [p=*this] (auto &&range) {
            return p.apply(std::forward<decltype(range)>(range), run_par_sync);
        }, std::forward<Range>(range));
    }

    template<class R, class Range>
    auto apply_sync(Range &&range, b0::meta::req_t<!std::is_void_v<R>, std::size_t> size) const -> auto
    {
        std::vector<R> results;
        if (size != 0) {
            const std::size_t concurrency = static_cast<std::size_t>(b0::ideal_concurrency());
            auto pos = std::cbegin(range);
            const auto blocks = size / concurrency;
            const auto remainder = size % concurrency;
            std::vector<std::thread> threads;
            results.resize(static_cast<std::size_t>(concurrency));
            auto out = results.begin();
            threads.reserve(static_cast<std::size_t>(concurrency - 1));
            auto run = [&] (auto begin, auto end, std::size_t count, auto out) {
                *out = this->apply(view(std::move(begin), std::move(end), count), run_seq);
            };
            auto push = [&] (std::size_t count) {
                auto begin = pos;
                std::advance(pos, count);
                auto end = pos;
                threads.emplace_back(run, std::move(begin), std::move(end), count, out);
                ++out;
                assert(size >= count);
                size -= count;
            };
            for (std::size_t i = 0; i < remainder; ++i)
                push(blocks + 1);
            for (std::size_t i = remainder; i < concurrency - 1; ++i)
                push(blocks);
            run(pos, std::end(std::forward<decltype(range)>(range)), size, out);
            for (auto &t : threads)
                t.join();
        }
        return get<this->size() - 1>().parallel(std::move(results));
    }

    template<class R, class Range>
    auto apply_sync(Range &&range, b0::meta::req_t<std::is_void_v<R>, std::size_t> size) const -> void
    {
        if (size != 0) {
            const std::size_t concurrency = static_cast<std::size_t>(b0::ideal_concurrency());
            auto pos = std::cbegin(range);
            const auto blocks = size / concurrency;
            const auto remainder = size % concurrency;
            std::vector<std::thread> threads;
            threads.reserve(static_cast<std::size_t>(concurrency - 1));
            auto run = [&] (auto begin, auto end, std::size_t count) {
                this->apply(view(std::move(begin), std::move(end), count), run_seq);
            };
            auto push = [&] (std::size_t count) {
                auto begin = pos;
                std::advance(pos, count);
                auto end = pos;
                threads.emplace_back(run, std::move(begin), std::move(end), count);
                assert(size >= count);
                size -= count;
            };
            for (std::size_t i = 0; i < remainder; ++i)
                push(blocks + 1);
            for (std::size_t i = remainder; i < concurrency - 1; ++i)
                push(blocks);
            run(pos, std::end(std::forward<decltype(range)>(range)), size);
            for (auto &t : threads)
                t.join();
        }
    }

    template<class Range>
    auto apply(Range &&range, run_par_sync_t) const -> auto
    {
        using Iterable = decltype(make_iterable<0, i_back>(wrap(range)));
        using R = std::decay_t<decltype(get<i_back>().run(std::declval<Iterable>()))>;
        std::size_t size = static_cast<std::size_t>(std::size(std::forward<Range>(range)));
        return apply_sync<R>(std::forward<Range>(range), size);
    }

    template<class Range>
    constexpr auto operator () (Range &&range) const -> auto
    { return apply(std::forward<Range>(range),
                   typename std::tuple_element_t<this->size() - 1, FnTuple>::run_policy()); }
#pragma warning(pop)
private:

#pragma warning(push)
#pragma warning(disable: 4100)
    template<int I, int N, class Range>
    constexpr auto make_iterable(Range &&range) const -> auto
    { return make_iterable<I>(std::forward<Range>(range), meta::make_int_seq<N>()); }
    template<int I, class Range, int... Ints>
    constexpr auto make_iterable(Range &&range, meta::int_seq<Ints...>) const -> auto
    { return make_iterable<I + 1, sizeof...(Ints) - 1>(get<I>().make_iterable(std::forward<Range>(range))); }
    template<class Range>
    constexpr auto make_range_iterable(Range &&range) const -> auto
    { return make_iterable<0, i_back>(wrap(std::forward<Range>(range))); }
#pragma warning(pop)
    template<int I, class Range>
    constexpr auto make_iterable(Range &&range, meta::int_seq<>) const -> std::decay_t<Range>
        { return std::forward<Range>(range); }

    template<int I>
    constexpr auto get() const& -> const std::tuple_element_t<I, FnTuple>&
        { return std::get<I>(this->tuple()); }
    template<int I>
    constexpr auto get() && -> std::tuple_element_t<I, FnTuple>&&
        { return std::get<I>(std::move(this->tuple())); }
};

template<class Fn>
constexpr inline auto make_pipe(Fn &&fn)
-> pipe_fn<std::tuple<std::decay_t<Fn>>, meta::is_terminal<std::decay_t<Fn>>::value>
{ return { in_place, std::forward<Fn>(fn) }; }

template<class Range, class Pipe>
struct range_pipe_fn {
    META_ASSERT(meta::is_range<Range>::value);

    template<class Rng, class P>
    range_pipe_fn(Rng &&range, P &&pipe)
        : m_range(std::forward<Rng>(range))
        , m_pipe(std::forward<P>(pipe)) {}

    template<class P>
    constexpr auto operator | (P &&pipe) const& -> auto
    { return make_range_pipe(m_range, m_pipe | std::forward<P>(pipe)); }

    template<class P>
    auto operator | (P &&pipe) && -> auto
    { return make_range_pipe(std::move(m_range), std::move(m_pipe) | std::forward<P>(pipe)); }
private:
    Range m_range;
    Pipe m_pipe;
};

namespace detail {
template<class Range, class Pipe>
constexpr auto make_range_pipe(Range &&range, Pipe &&pipe, meta::true_)
-> meta::invoke_t<Pipe&&, Range&&>
{ return std::forward<Pipe>(pipe)(std::forward<Range>(range)); }
template<class Range, class Pipe>
constexpr auto make_range_pipe(Range &&range, Pipe &&pipe, meta::false_)
-> range_pipe_fn<std::decay_t<Range>, std::decay_t<Pipe>>
{ return { std::forward<Range>(range), std::forward<Pipe>(pipe) }; }
}


template<class Range, class Pipe>
constexpr auto make_range_pipe(Range &&range, Pipe &&pipe) -> auto
{
    using done = meta::bool_<std::decay_t<Pipe>::is_terminated>;
    return detail::make_range_pipe(std::forward<Range>(range), std::forward<Pipe>(pipe), done());
}

}

}
