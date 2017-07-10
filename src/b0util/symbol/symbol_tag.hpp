#pragma once

#include "../meta/meta.hpp"

namespace b0 { namespace symbol { template<class Tag> struct smbl; } }

#define b0_define_symbol_tag(symname) \
namespace s { struct __tag_ ## symname ## _t { \
    using symbol_t = b0::symbol::smbl<__tag_ ## symname ## _t>; \
    static constexpr const char name[] = #symname; \
    template<class T> struct holder { T symname; }; \
    template<class T> static constexpr auto get(const T &t) -> const decltype(t.symname)& { return t.symname; } \
    template<class T> static constexpr auto get(T &t) -> decltype(t.symname)& { return t.symname; } \
    template<class T> static constexpr auto get(T &&t) -> decltype(t.symname)&& { return std::move(t.symname); } \
    template<class T, class... Args> static constexpr auto invoke(T &&t, Args&&... args) \
    -> decltype(std::forward<T>(t).symname(std::forward<Args>(args)...)) \
    { return std::forward<T>(t).symname(std::forward<Args>(args)...); } \
    B0_DECLARE_HAS_MEMBER_WITH_NAME(has_symbol, symname) }; }
