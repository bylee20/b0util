#pragma once

#include "../global.hpp"
#include "../utility.hpp"
#include "../meta/meta.hpp"

namespace b0 { namespace symbol {

template<class Tag> struct smbl;
namespace detail {template <class Tag, class T, class Opts> struct holder_op;}

/******************************************************************************/

template <class Tag, class T, class Opts>
struct holder : Tag::template holder<T> {
  template <class V, class O>
  holder(V &&other, O &&opts)
      : Tag::template holder<T>{std::forward<V>(other)},
        __symbol_holder_opts(std::forward<O>(opts)) {}
  B0_CONSTEXPR_DEFAULT_COPY_MOVE(holder);

private:
  friend struct detail::holder_op<Tag, T, Opts>;
  Opts __symbol_holder_opts;
};

template <class Tag, class T>
struct holder<Tag, T, void> : Tag::template holder<T> {
    template <class V>
    holder(V &&other)
        : Tag::template holder<T>{std::forward<V>(other)}
          {}
    B0_CONSTEXPR_DEFAULT_COPY_MOVE(holder);
};

/******************************************************************************/

template<class... Holders>
struct object : Holders...
{
    template<class... Hs>
    object(b0::in_place_t, Hs&&... holders)
        : Holders(holders)... {}
};

/******************************************************************************/

template<class Tag, class... Holders>
struct holder_options
{
    template<class... Args>
    holder_options(b0::in_place_t, Args&&... args): m_options(b0::in_place, std::forward<Args>(args)...) {}

    template<class Sym>
    auto get(const Sym&) const -> const auto& { return get<Sym>(); }
    template<class Sym>
    auto get() const -> const auto& { return Sym::get(m_options); }
    template<class Sym, class V>
    auto get_or(const Sym&, V &&def) const -> decltype(auto) { return get_or<Sym>(std::forward<V>(def)); }
    template<class Sym, class V>
    auto get_or(V &&def) const -> decltype(auto) { return Sym::get_or(m_options, std::forward<V>(def)); }

    template<class Sym>
    auto exists() const -> bool { return Sym::exists(m_options); }
    template<class Sym>
    auto exists(const Sym&) const -> bool { return exists<Sym>(); }

    template<class T>
    auto operator = (T &&t) && -> auto
    { return holder<Tag, std::decay_t<T>, holder_options>(std::forward<T>(t), std::move(*this)); }
private:
    object<Holders...> m_options;
};

/******************************************************************************/

namespace detail {

struct empty_t {};

template<class T>
struct to_holder_impl;

template<class Tag>
struct to_holder_impl<smbl<Tag>> {
    template<class V>
    static auto apply(V &&symbol) -> auto { return symbol = empty_t(); }
};

template<class Tag, class T, class Opts>
struct to_holder_impl<holder<Tag, T, Opts>> {
    template<class V>
    static auto apply(V &&holder) -> decltype(auto) { return std::forward<V>(holder); }
};

template<class Tag, class... Holders>
struct to_holder_impl<holder_options<Tag, Holders...>> {
    template<class V>
    static auto apply(V &&options) -> auto { return std::forward<V>(options) = empty_t(); }
};

template<class T>
inline auto to_holder(T &&t) -> decltype(auto)
{
    return detail::to_holder_impl<std::decay_t<T>>::apply(std::forward<T>(t));
}

template <class Tag, class T, class Opts>
struct holder_op {
  using holder = b0::symbol::holder<Tag, T, Opts>;

  template <class H> static auto options(const H &h) -> const auto & {
    return h.__symbol_holder_opts;
  }
};

template <class Tag, class T>
struct holder_op<Tag, T, void> {
  template <class H>
  static auto options(const H &) -> auto { return holder_options<Tag>(b0::in_place); }
};

template <class Tag, class T, class Opts>
inline auto options(const holder<Tag, T, Opts> &h) -> auto {
  return detail::holder_op<Tag, T, Opts>::options(h);
}

template<class Tag, class... Holders>
inline auto to_options(Holders &&... holders) -> auto
{
    return holder_options<Tag, std::decay_t<Holders>...>{b0::in_place, std::forward<Holders>(holders)...};
}

template<class... Holders>
inline auto to_object(Holders &&... holders) -> auto
{
    return object<std::decay_t<Holders>...>(b0::in_place, std::forward<Holders>(holders)...);
}

template<class Tag, class H, class... Hs>
struct find_holder
{
    static_assert(sizeof...(Hs) > 0, "cannot find symbol!");
    using type = typename find_holder<Tag, Hs...>::type;
};

template<class Tag, class T, class Opts, class... Hs>
struct find_holder<Tag, holder<Tag, T, Opts>, Hs...> {
    using type = holder<Tag, T, Opts>;
};

}

template<class Tag, class T, class Opts>
constexpr inline auto symbol(const holder<Tag, T, Opts>&) -> b0::symbol::smbl<Tag> { return b0::symbol::smbl<Tag>(); }

template <class Sym, class... Holders>
auto options(const object<Holders...> &o, const Sym&) -> auto {
  return detail::options(static_cast<const typename detail::find_holder<typename Sym::tag, Holders...>::type&>(o));
}

template<class... Args>
inline auto make_object(Args&&...args) -> auto
{
    return detail::to_object(detail::to_holder(std::forward<Args>(args))...);
}

template<class Tag, class... Args>
inline auto make_options(Args&&... args) -> auto
{
    return detail::to_options<Tag>(detail::to_holder(std::forward<Args>(args))...);
}

template<class... Args>
inline auto make_options(Args&&... args) -> auto
{
    return make_options<void>(std::forward<Args>(args)...);
}

template<class... Holders>
constexpr inline auto size(const object<Holders...>&) { return sizeof...(Holders); }

template<class Fn>
inline auto for_each(const object<> &, Fn &&) -> void {}

template<class... Holders, class Fn>
inline auto for_each(const object<Holders...> &o, Fn &&fn) -> void
{
    b0_expand(std::forward<Fn>(fn)(static_cast<const Holders&>(o)));
}

template<class... Holders, class Fn>
inline auto for_each(object<Holders...> &o, Fn &&fn) -> void
{
    b0_expand(std::forward<Fn>(fn)(static_cast<Holders&>(o)));
}

template<class... Holders, class Fn>
inline auto for_each(object<Holders...> &&o, Fn &&fn) -> void
{
    b0_expand(std::forward<Fn>(fn)(static_cast<Holders&&>(o)));
}

}
}
