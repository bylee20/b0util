//#pragma once

//#include "./utility.hpp"
//#include "./fp.hpp"
//#include "./algorithm.hpp"
//#include "./meta/meta.hpp"
//#include "./meta/container.hpp"

//namespace b0 {

//template<class Begin, class End>
//struct range_view {
//    template<class _Begin, class _End>
//    range_view(_Begin &&begin, _End &&end)
//        : m_begin(std::forward<_Begin>(begin))
//        , m_end(std::forward<_End>(end))
//        , m_size(std::distance(m_begin, m_end)) {}
//    auto begin() const -> const Begin& { return m_begin; }
//    auto end() const -> const End& { return m_end; }
//    auto cbegin() const -> const Begin& { return m_begin; }
//    auto cend() const -> const End& { return m_end; }
//    auto size() const -> int { return m_size; }
//    auto empty() const -> bool { return m_begin != m_end; }
//private:
//    Begin m_begin;
//    End m_end;
//    int m_size = 0;
//};

//template<class Begin, class End>
//constexpr inline auto view(Begin &&begin, End &&end)
//-> meta::req_t<meta::is_iterator_v<std::decay_t<Begin> >,
//    range_view<std::decay_t<Begin>, std::decay_t<End> > >
//{
//    using range = range_view<std::decay_t<Begin>, std::decay_t<End> >;
//    return range(std::forward<Begin>(begin), std::forward<End>(end));
//}

//template<class Begin>
//constexpr inline auto view(Begin &&begin, int n)
//-> meta::req_t<meta::is_iterator_v<std::decay_t<Begin>>,
//    decltype(view(std::forward<Begin>(begin), std::move(begin + n)))>
//{
//    std::decay_t<Begin> end = std::forward<Begin>(begin);
//    std::advance(end, n);
//    return view(std::forward<Begin>(begin), std::move(end));
//}

//template<class T>
//constexpr inline auto view(T &&t) -> decltype(view(std::begin(std::forward<T>(t)), std::end(std::forward<T>(t))))
//{ return view(std::begin(std::forward<T>(t)), std::end(std::forward<T>(t))); }

//namespace detail {
//template<class C, int... Ints>
//constexpr inline auto ints_as_impl(meta::int_seq<Ints...>) -> C { return C{Ints...}; }
//}

//template<class C, int begin, int end>
//constexpr inline auto ints_as() -> C
//{ return detail::ints_as_impl<C>(meta::make_int_range_seq<begin, end>()); }

//template<class C, int size>
//constexpr inline auto ints_as() -> C
//{ return detail::ints_as_impl<C>(meta::make_int_seq<size>()); }

//template<int begin, int end>
//constexpr inline auto ints() -> auto
//{ return ints_as<std::vector<int>, begin, end>(); }

//template<int size>
//constexpr inline auto ints() -> auto
//{ return ints_as<std::vector<int>, size>(); }

//template<class C, class T = int>
//constexpr inline auto ints_as(T begin, T end) -> C
//{
//    C c;
//    try_reserve(c, end - begin);
//    for (auto i = begin; i != end; ++i)
//        emplace_or_push_back(c, i);
//    return c;
//}

//template<class C, class T = int>
//constexpr inline auto ints_as(T size) -> C { return ints_as<C, T>(static_cast<T>(0), size); }

//template<class T = int>
//constexpr inline auto ints(T begin, T end) -> std::vector<T> { return ints_as<std::vector<T>>(begin, end); }

//template<class T>
//constexpr inline auto ints(T size) -> std::vector<T> { return ints<T>(0, size); }

//template<class... Fs>
//struct range_fn_pipe;

//template<class T>
//struct range_fn;

//template<class F1, class F2>
//constexpr inline auto make_fn_pipe(F1 &&f1, F2 &&f2) -> auto;

//template<class Range, class T>
//struct range_pipe {
//    template<class _Range, class _T>
//    range_pipe(_Range &&range, _T &&t)
//        : m_range(std::forward<_Range>(range))
//        , m_pipe(std::forward<_T>(t)) {}
//    template<class _T>
//    constexpr inline auto operator | (range_fn<_T> &&rhs) && -> auto
//    {
//        auto &&pipe = (std::move(m_pipe) | static_cast<_T&&>(rhs));
//        return range_pipe<Range, std::decay_t<decltype(pipe)>>(std::move(m_range), std::move(pipe));
//    }
//    template<class _T>
//    constexpr inline auto operator | (const range_fn<_T> &rhs) && -> auto
//    {
//        auto &&pipe = (std::move(m_pipe) | static_cast<const _T&>(rhs));
//        return range_pipe<Range, std::decay_t<decltype(pipe)>>(std::move(m_range), std::move(pipe));
//    }
//    template<class _T>
//    constexpr inline auto operator | (range_fn<_T> &&rhs) const& -> auto
//    {
//        auto &&pipe = (m_pipe | static_cast<_T&&>(rhs));
//        return range_pipe<Range, std::decay_t<decltype(pipe)>>(std::move(m_range), std::move(pipe));
//    }
//    template<class _T>
//    constexpr inline auto operator | (const range_fn<_T> &rhs) const& -> auto
//    {
//        auto &&pipe = (m_pipe | static_cast<const _T&>(rhs));
//        return range_pipe<Range, std::decay_t<decltype(pipe)>>(std::move(m_range), std::move(pipe));
//    }
//private:
//    Range m_range;
//    T m_pipe;
//};

//template<class... Fns>
//struct range_fn_pipe
//{
//    template<class... Args>
//    range_fn_pipe(Args&&... args): m_fns(std::forward<Args>(args)) {}
//    template<class Range>
//    constexpr auto operator () (Range &&range) const& -> decltype(auto)
//    { return m_f2(m_f1(std::forward<Range>(range))); }
//    template<class Range>
//    constexpr auto operator () (Range &&range) && -> decltype(auto)
//    { return std::move(m_f2)(std::move(m_f1)(std::forward<Range>(range))); }
//private:
//    std::tuple<Fns...> m_fns;
//};

//template<class T>
//struct range_fn
//{
//    template<class Range, B0_REQ(b0::meta::is_range_v<Range>)>
//    friend constexpr inline auto operator | (Range &&range, const range_fn<T> &fn) -> decltype(auto)
//    { return range_pipe<std::decay_t<Range>, T>(std::forward<Range>(range), static_cast<const T&>(fn)); }
//    template<class Range, B0_REQ(b0::meta::is_range_v<Range>)>
//    friend constexpr inline auto operator | (Range &&range, range_fn<T> &&fn) -> decltype(auto)
//    { return range_pipe<std::decay_t<Range>, T>(std::forward<Range>(range), static_cast<T&&>(fn)); }

//    template<class U>
//    constexpr inline auto operator | (range_fn<U> &&rhs) const& -> decltype(auto)
//    { return b0::range_fn_pipe<T, U>(static_cast<const T&>(*this), static_cast<U&&>(rhs)); }
//    template<class U>
//    constexpr inline auto operator | (const range_fn<U> &rhs) const& -> decltype(auto)
//    { return b0::range_fn_pipe<T, U>(static_cast<const T&>(*this), static_cast<const U&>(rhs)); }
//    template<class U>
//    inline auto operator | (range_fn<U> &&rhs) && -> decltype(auto)
//    { return b0::range_fn_pipe<T, U>(static_cast<T&&>(*this), static_cast<U&&>(rhs)); }
//    template<class U>
//    inline auto operator | (const range_fn<U> &rhs) && -> decltype(auto)
//    { return b0::range_fn_pipe<T, U>(static_cast<T&&>(*this), static_cast<const U&>(rhs)); }

//};

//template<class T>
//struct is_range_fn : meta::false_ {};
//template<class T>
//struct is_range_fn<range_fn<T>> : meta::true_ {};

//template<class T>
//constexpr bool is_range_fn_v = is_range_fn<T>::value;

//template<class F1, class F2>
//struct fn_pipe
//{
//    template<class _F1, class _F2>
//    constexpr fn_pipe(_F1 &&f1, _F2 &&f2)
//        : m_f1(std::forward<_F1>(f1)), m_f2(std::forward<_F2>(f2)) {}
//    template<class... Args>
//    constexpr auto operator() (Args&&... args) const& -> decltype(auto)
//    { return m_f2(m_f1(std::forward<Args>(args)...)); }
//    template<class... Args>
//    constexpr auto operator() (Args&&... args) && -> decltype(auto)
//    { return std::move(m_f2)(std::move(m_f1)(std::forward<Args>(args)...)); }
//private:
//    F1 m_f1; F2 m_f2;
//};

//template<class F1, class F2>
//constexpr inline auto make_fn_pipe(F1 &&f1, F2 &&f2) -> auto
//{ return fn_pipe<std::decay_t<F1>, std::decay_t<F2>>(std::forward<F1>(f1), std::forward<F2>(f2)); }

////template<class F1, class F2>
////struct range_fn_pipe : range_fn<range_fn_pipe<F1, F2>>
////{
////    template<class _F1, class _F2>
////    constexpr range_fn_pipe(_F1 &&f1, _F2 &&f2)
////        : m_f1(std::forward<_F1>(f1)), m_f2(std::forward<_F2>(f2)) {}
////    template<class Range>
////    auto operator () (Range &&range) const& -> decltype(auto)
////    { return m_f2(m_f1(std::forward<Range>(range))); }
////    template<class Range>
////    auto operator () (Range &&range) && -> decltype(auto)
////    { return std::move(m_f2)(std::move(m_f1)(std::forward<Range>(range))); }
////private:
////    F1 m_f1; F2 m_f2;
////};

////template<class C, class F>
////struct map_as_fn : range_fn<map_as_fn<C, F>>
////{
////    template<class _F>
////    map_as_fn(_F &&func): m_func(std::forward<_F>(func)) {}
////    template<class Range>
////    auto operator() (Range &&range) const& -> decltype(auto)
////    { return map_as<C>(std::forward<Range>(range), m_func); }
////    template<class Range>
////    auto operator() (Range &&range) && -> decltype(auto)
////    { return map_as<C>(std::forward<Range>(range), std::move(m_func)); }
////private:
////    F m_func;
////};

//template<class F>
//struct map_fn : range_fn<map_fn<F>>
//{
//    template<class _F>
//    map_fn(_F &&func): m_func(std::forward<_F>(func)) {}

////    template<class Range>
////    auto operator() (Range &&range) const& -> decltype(auto)
////    { return b0::map(std::forward<Range>(range), m_func); }
////    template<class Range>
////    auto operator() (Range &&range) && -> decltype(auto)
////    { return b0::map(std::forward<Range>(range), std::move(m_func)); }

//    template<class C, class T>
//    constexpr auto apply(C &c, T &&t) const -> void
//    { emplace_or_push_back(c, m_func(std::forward<T>(t))); }

//    template<class _F>
//    constexpr auto pipe(map_fn<_F> &&other) && -> auto
//    {
//        auto &&pipe = make_fn_pipe(std::move(m_func), std::move(other));
//        return map_fn<std::decay_t<decltype(pipe)>>(std::move(pipe));
//    }
//private:
//    template<class _F>
//    friend struct map_fn;
//    F m_func;
//};



//template<class F>
//struct filter_fn : range_fn<filter_fn<F>>
//{
//    template<class _F>
//    filter_fn(_F &&func): m_func(std::forward<_F>(func)) {}

//    template<class C, class T>
//    constexpr auto apply(C &c, T &&t) const -> void
//    { if (m_func(const_(t))) emplace_or_push_back(c, std::forward<T>(t)); }
//private:
//    F m_func;
//};

////template<class C, class F>
////constexpr inline auto map_as(F &&func) -> auto
////{ return map_as_fn<C, std::decay_t<F>>(std::forward<F>(func)); }

////template<class F>
////constexpr inline auto map(F &&func) -> auto
////{ return map_fn<std::decay_t<F>>(std::forward<F>(func)); }

////template<class C, class F>
////struct filter_as_fn : range_fn<filter_as_fn<C, F>>
////{
////    template<class _F>
////    filter_as_fn(_F &&func): m_func(std::forward<F>(func)) {}
////    template<class Range>
////    auto operator() (Range &&range) const& -> decltype(auto)
////    { return filter_as<C>(std::forward<Range>(range), m_func); }
////    template<class Range>
////    auto operator() (Range &&range) && -> decltype(auto)
////    { return filter_as<C>(std::forward<Range>(range), std::move(m_func)); }
////private:
////    F m_func;
////};

////template<class C, class F>
////constexpr inline auto filter_as(F &&func) -> auto
////{ return filter_as_fn<C, std::decay_t<F>>(std::forward<F>(func)); }

////template<class F>
////struct filter_fn : range_fn<filter_fn<F>>
////{
////    template<class _F>
////    filter_fn(_F &&func): m_func(std::forward<F>(func)) {}
////    template<class Range>
////    auto operator() (Range &&range) const& -> decltype(auto)
////    { return filter(std::forward<Range>(range), m_func); }
////    template<class Range>
////    auto operator() (Range &&range) && -> decltype(auto)
////    { return filter(std::forward<Range>(range), std::move(m_func)); }
////private:
////    F m_func;
////};

////template<class F>
////constexpr inline auto filter(F &&func) -> auto
////{ return filter_fn<std::decay_t<F>>(std::forward<F>(func)); }

//}
