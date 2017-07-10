#pragma once

#include "../meta/meta.hpp"
#include "symbol_tag.hpp"
#include "object.hpp"

#define b0_define_symbol(name) \
    b0_define_symbol_tag(name); namespace s { \
        using _ ## name ## _t = ::b0::symbol::smbl<__tag_ ## name ## _t>; \
        constexpr _ ## name ## _t _ ## name; }

namespace b0 { namespace symbol {

template<class Tag>
struct smbl {
    using tag = Tag;

    template<class T>
    static constexpr bool exists_v = Tag::template has_symbol<b0::meta::uncref_t<T> >::value;

    template<class T, class V>
    static constexpr bool can_match_v = exists_v<T> && exists_v<V>;

    constexpr smbl() = default;

    static constexpr auto name() -> const auto& { return Tag::name; }

    template<class T>
    auto operator = (T &&t) const -> holder<Tag, std::decay_t<T>, void> { return holder<Tag, std::decay_t<T>, void>{std::forward<T>(t)}; }

    template<class... Args>
    auto operator () (Args&&... args) const -> auto
    {
        return make_options<Tag>(detail::to_holder(std::forward<Args>(args))...);
    }

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

    static constexpr auto equals(smbl) -> bool { return true; }
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

}}
