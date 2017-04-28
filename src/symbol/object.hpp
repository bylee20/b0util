#pragma once

#include "./symbol.hpp"
#include "../meta/type_seq.hpp"
#include "../fp.hpp"

namespace b0 {

namespace symbol {

template<class... Holders>
struct object;

template<class T>
struct symbol_seq;

template<class... Holders>
struct symbol_seq<object<Holders...>>
    : meta::wrap_type<meta::type_seq<symbol_type_t<Holders>...>> {};

template<class T>
using symbol_seq_t = typename symbol_seq<T>::type;

template<class Holder, class Opts>
struct holder_factory;

template<class... Holders, class... _Holders>
constexpr inline auto compare(const object<Holders...> &lhs, const object<_Holders...> &rhs)
-> meta::req_t<sizeof...(Holders) == sizeof...(_Holders), bool>;

template<class... Holders, class... _Holders>
constexpr inline auto compare(const object<Holders...> &, const object<_Holders...> &)
-> meta::req_t<sizeof...(Holders) != sizeof...(_Holders), bool> { return false; }

template<class... Holders>
struct object : Holders...
{
    META_ASSERT(meta::type_seq<Holders...>::exclusive);
    META_ASSERT(meta::count_type_if_v<meta::type_seq<Holders...>, is_holder> == sizeof...(Holders));

    template<class... Args>
    constexpr object(Args&&... args): Holders(std::forward<Args>(args))... { }
    B0_CONSTEXPR_DEFAULT_COPY_MOVE(object);

    template<class T>
    constexpr auto operator == (const T &rhs) const -> bool { return compare(*this, rhs); }
    template<class T>
    constexpr auto operator != (const T &rhs) const -> bool { return !(*this == rhs); }
};

template<int I, class U, class T = std::decay_t<U>,
         class SymSeq = symbol_seq_t<T>,
         class Sym = typename SymSeq::template get<I>>
constexpr inline auto get(U &&t) -> decltype(auto) { return Sym::get(std::forward<U>(t)); }

namespace detail {

using namespace b0::meta;

template<class F, class T, int... Ints>
auto for_each_impl(int_seq<Ints...>, T &&t, const F &f) -> void
{ b0_expand(f(b0::get<Ints>(std::forward<T>(t)))); }

template<int, class T, class Arg>
auto for_each_assign_at(T &t, Arg &&arg) -> req_t<is_holder_v<std::decay_t<Arg> > >
{
    using Sym = symbol_type_t<std::decay_t<Arg>>;
    Sym::get(t) = Sym::get(std::forward<Arg>(arg));
}

template<int I, class T, class Arg>
auto for_each_assign_at(T &t, Arg &&arg) -> req_t<!is_holder_v<std::decay_t<Arg> > >
{ b0::symbol::get<I>(t) = std::forward<Arg>(arg); }

template<class T, int... Ints, class... Args>
auto for_each_assign_impl(int_seq<Ints...>, T &t, Args&&... args) -> void
{ b0_expand(for_each_assign_at<Ints>(t, b0::get<Ints>(std::forward<Args>(args)...))); }

template<class SymSeq, int I, int N, bool = (I < N && 1 > 0)>
struct for_each_compare {
    template<class L, class R, class Sym = typename SymSeq::template get<I>>
    static constexpr auto apply(const L &l, const R &r) -> bool
    { return Sym::get(l) == Sym::get(r) && for_each_compare<SymSeq, I + 1, N>::apply(l, r); }
};

template<class SymSeq, int I, int N>
struct for_each_compare<SymSeq, I, N, false> {
    template<class L, class R>
    static constexpr auto apply(const L &, const R &) -> bool { return true; }
};

}

template<class... Args>
inline auto make_object(Args&&... args) -> auto
{ return object<std::decay_t<Args>...>{std::forward<Args>(args)...}; }

template<class... Holders, class... Args>
constexpr inline auto assign(object<Holders...> &o, Args&&... args) -> void
{ detail::for_each_assign_impl(meta::int_seq_for<Args...>(), o, std::forward<Args>(args)...); }

template<class F, class... Holders>
auto for_each(object<Holders...> &&t, const F &f) -> void
{ detail::for_each_impl(meta::int_seq_for<Holders...>(), std::move(t), f); }

template<class F, class... Holders>
auto for_each(const object<Holders...> &t, F &&f) -> void
{ detail::for_each_impl(meta::int_seq_for<Holders...>(), t, std::forward<F>(f)); }

template<class F, class... Holders>
auto for_each(object<Holders...> &t, F &&f) -> void
{ detail::for_each_impl(meta::int_seq_for<Holders...>(), t, std::forward<F>(f)); }

template<class... Holders, class... _Holders>
constexpr inline auto compare(const object<Holders...> &lhs, const object<_Holders...> &rhs)
-> meta::req_t<sizeof...(Holders) == sizeof...(_Holders), bool>
{ return detail::for_each_compare<symbol_seq_t<object<Holders...>>, 0, sizeof...(Holders)>::apply(lhs, rhs); }

}}
