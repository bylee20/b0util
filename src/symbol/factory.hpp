#pragma once

#include "./object.hpp"
#include <tuple>

namespace b0 { namespace symbol {

template<class Holder, class Opts>
struct holder_factor;

template<class... HFs>
class object_factory;

template<class Sym, class... Holders>
struct options : object<Holders...>
{
    using object<Holders...>::object;
    template<class U>
    auto operator = (U &&t) && -> auto
    {
        using T = std::decay_t<U>;
        using Holder = typename Sym::template holder<T>;
        using Factory = holder_factory<Holder, options>;
        return Factory(Sym() = std::forward<T>(t), std::move(*this));
    }
};

namespace detail {

template<class Holder>
struct holder_factory_base
{
    using symbol_type = symbol_type_t<Holder>;
    template<class U>
    holder_factory_base(U &&u)
        : m_holder(std::forward<U>(u)) {}
    B0_CONSTEXPR_DEFAULT_COPY_MOVE(holder_factory_base);
    auto get() & -> auto& { return m_holder; }
    auto get() && -> auto&& { return std::move(m_holder); }
    auto get() const& -> const auto& { return m_holder; }
private:
    Holder m_holder;
};

}

template<class Holder, class Opts>
struct holder_factory : detail::holder_factory_base<Holder>
{
    template<class U>
    holder_factory(U &&u, Opts &&opts)
        : detail::holder_factory_base<Holder>(std::forward<U>(u))
        , m_options(std::move(opts)) {}
    template<class S, class U>
    auto option(S, U &&u) const -> auto { return option<S>(std::forward<U>(u)); }
    template<class S, class U>
    auto option(U &&u) const -> auto { return S::get_or(m_options, std::forward<U>(u)); }
    template<class S>
    auto option() const -> auto { return S::get(m_options); }
private:
    Opts m_options;
};

template<class Holder>
struct holder_factory<Holder, void> : detail::holder_factory_base<Holder>
{
    using detail::holder_factory_base<Holder>::holder_factory_base;
    template<class S, class U>
    auto option(S, U &&u) const -> auto { return std::forward<U>(u); }
    template<class S, class U>
    auto option(U &&u) const -> auto { return std::forward<U>(u); }
};

template<class Holder>
inline constexpr auto make_holder_factory(Holder &&holder) -> auto
{
    META_ASSERT(is_holder_v<Holder>);
    return holder_factory<Holder, void>(std::move(holder));
}

template<class Holder, class Opts>
inline constexpr auto make_holder_factory(holder_factory<Holder, Opts> &&factory) -> auto
{ return std::move(factory); }

template<class... Factories>
class object_factory
{
    using symbol_seq = meta::type_seq<typename Factories::symbol_type...>;
public:
    using object_type = std::decay_t<decltype(make_object(std::declval<Factories>().get()...))>;
    template<class... _F>
    object_factory(_F&&... f)
        : m_symbols(std::forward<_F>(f)...) {}
//    B0_DEFAULT_COPY_MOVE(object_factory);
    object_factory(const object_factory &rhs)
        : m_symbols(rhs.m_symbols) {}
    object_factory(object_factory &&rhs)
        : m_symbols(std::move(rhs.m_symbols)) {}

    template<class Sym>
    constexpr auto info() const -> const auto&
    { return std::get<meta::type_index_v<Sym, symbol_seq>>(m_symbols); }
    template<class Sym>
    constexpr auto info(Sym) const -> const auto& { return info<Sym>(); }
    auto operator () () && -> auto { return make(meta::int_seq_for<Factories...>()); }
    auto operator () () const& -> auto { return make(meta::int_seq_for<Factories...>()); }

    template<class Tag>
    auto operator () (symbol<Tag>) const -> const auto& { return info<symbol<Tag>>(); }
private:
    template<int I>
    constexpr auto make_holder() const& -> decltype(auto) { return std::get<I>(m_symbols).get(); }
    template<int I>
    auto make_holder() && -> decltype(auto) { return std::get<I>(std::move(m_symbols)).get(); }
    template<int... Ints>
    auto make(meta::int_seq<Ints...>) const& -> auto
    { return b0::symbol::make_object(make_holder<Ints>()...); }
    template<int... Ints>
    auto make(meta::int_seq<Ints...>) && -> auto
    { return b0::symbol::make_object(make_holder<Ints>()...); }
    std::tuple<Factories...> m_symbols;
};

template<class... Args>
constexpr inline auto make_factory(Args&&... args) -> auto
{
    using Factory = object_factory<decltype(make_holder_factory(std::forward<Args>(args)))...>;
    return std::move(Factory(make_holder_factory(std::forward<Args>(args))...));
}

template<class Tag, class Factory>
struct tagged_factory : Factory {
    using Factory::Factory;
};



}}

