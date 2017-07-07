#pragma once

#include "../meta/meta.hpp"

#define b0_define_symbol_tag(symname) \
namespace s { struct __tag_ ## symname ## _t { \
    static constexpr const char name[] = #symname; \
    template<class T> struct holder { \
        template<class... Args> constexpr holder(Args&&... args) \
            : symname(std::forward<Args>(args)...) { } \
        B0_CONSTEXPR_DEFAULT_COPY_MOVE(holder); T symname; }; \
    template<class T> static constexpr auto get(const T &t) -> const decltype(t.symname)& { return t.symname; } \
    template<class T> static constexpr auto get(T &t) -> decltype(t.symname)& { return t.symname; } \
    template<class T> static constexpr auto get(T &&t) -> decltype(t.symname)&& { return std::move(t.symname); } \
    template<class T, class... Args> static constexpr auto invoke(T &&t, Args&&... args) \
    -> decltype(std::forward<T>(t).symname(std::forward<Args>(args)...)) \
    { return std::forward<T>(t).symname(std::forward<Args>(args)...); } \
    B0_DECLARE_HAS_MEMBER_WITH_NAME(has_symbol, symname) }; }

#define b0_define_symbol(name) \
    b0_define_symbol_tag(name); namespace s { \
        using _ ## name ## _t = ::b0::symbol::symbol<__tag_ ## name ## _t>; \
        constexpr _ ## name ## _t _ ## name; }

namespace b0 {

namespace symbol {

template<class Sym>
struct holder { };

template<class Sym, class... Holders>
struct options;

template<class Tag>
struct symbol {
    template<class T>
    using base_holder_type = typename Tag::template holder<T>;

    template<class T>
    struct holder : b0::symbol::holder<symbol>, Tag::template holder<T>
    {
        using __holding_symbol_type = symbol;
        using Tag::template holder<T>::holder;
    };

    template<class T>
    static constexpr bool exists_v = Tag::template has_symbol<b0::meta::uncref_t<T> >::value;

    template<class T, class V>
    static constexpr bool can_match_v = exists_v<T> && exists_v<V>;

    constexpr symbol() = default;

    static constexpr auto name() -> const auto& { return Tag::name; }

    template<class T>
    auto operator = (T &&t) const -> auto { return holder<std::decay_t<T>>{std::forward<T>(t)}; }

    template<class... Args>
    auto operator () (Args&&... args) const -> auto
    { return options<symbol, std::decay_t<Args>...>{std::forward<Args>(args)...}; }

    template<class T>
    static constexpr auto get(T &&t) -> decltype(Tag::get(std::forward<T>(t))) { return Tag::get(std::forward<T>(t)); }

    template<class V, class T>
    static constexpr auto get_or(T &&t, V&& = {}) -> decltype(get(std::forward<T>(t)))
    { return get(std::forward<T>(t)); }
    template<class V, class T>
    static constexpr auto get_or(T &&, V &&def= {}) -> b0::meta::req_t<!exists_v<T>, std::decay_t<V> > { return std::forward<V>(def); }

    template<class T, class... Args>
    static constexpr auto invoke(T &&t, Args&&... args) -> decltype(auto)
    { return Tag::invoke(std::forward<T>(t), std::forward<Args>(args)...); }

    static constexpr auto equals(symbol) -> bool { return true; }
    template<class T>
    static constexpr auto equals(T&&) -> bool { return false; }

    template<class T>
    static constexpr auto exists() -> bool { return exists_v<T>; }
    template<class T>
    static constexpr auto exists(T &&) -> bool { return exists_v<T>; }

    template<class T, class V>
    using req_match_t = b0::meta::req_t<can_match_v<T, V>, bool>;
    template<class T, class V>
    using req_not_match_t = b0::meta::req_t<!can_match_v<T, V>, bool>;

    template<class T, class V>
    static constexpr auto try_assign(T &&t, V &&v) -> req_match_t<T, V>
    { get(std::forward<T>(t)) = get(std::forward<V>(v)); return true; }
    template<class T, class V>
    static constexpr auto try_assign(T &&, V &&) -> req_not_match_t<T, V> { return false; }

    template<class T, class V>
    static constexpr auto try_compare(const T &t, const V &v) -> req_match_t<T, V>
    { return get(t) == get(v); }
    template<class T, class V>
    static constexpr auto try_compare(const T &, const V &) -> req_not_match_t<T, V> { return false; }
};

template<class T>
struct is_symbol : meta::false_ {};

template<class T>
struct is_symbol<symbol<T>> : meta::true_ {};

namespace detail {

template<class T, class = void>
struct is_holder : meta::false_ {};

template<class T>
struct is_holder<T, decltype((void)std::declval<typename T::__holding_symbol_type>())> : meta::true_ {};

}

template<class T>
using is_holder = detail::is_holder<T>;

template<class T>
static constexpr bool is_symbol_v = is_symbol<T>::value;

template<class T>
static constexpr bool is_holder_v = is_holder<T>::value;

template<class T>
struct symbol_type : meta::wrap_type<typename T::__holding_symbol_type> {};

template<class Sym, class... Holders>
struct symbol_type<options<Sym, Holders...> > : meta::wrap_type<Sym> {};

template<class T>
using symbol_type_t = typename symbol_type<T>::type;

template<class Sym, class T>
constexpr inline auto get(T &&t, B0_REQ_OP(is_symbol_v<Sym>))
-> decltype(auto) { return Sym::get(std::forward<T>(t)); }

template<class Sym, class T>
constexpr inline auto get(Sym, T &&t, B0_REQ_OP(is_symbol_v<Sym>))
-> decltype(auto) { return b0::symbol::get<Sym>(std::forward<T>(t)); }

template<class Sym, class T>
constexpr inline auto has() -> bool { return Sym::template exists<T>(); }

template<class Sym, class T>
constexpr inline auto has(T&&) -> bool { return has<Sym, T>(); }

template<class Sym, class T>
constexpr inline auto has(Sym, T&&) -> bool { return has<Sym, T>(); }

}

}
