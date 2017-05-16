#pragma once

#include "meta/meta.hpp"
#include <utility>
#include <iostream>
#include <memory>

namespace b0 {

constexpr struct in_place_t { } in_place;

struct nullopt_t {
    explicit constexpr nullopt_t(int) {}
};

constexpr nullopt_t nullopt(0);

namespace detail {

template<class T>
struct optional_storage_value {
    auto has_value() const -> bool { return m_set; }
    auto value() const -> const T& { return m_value; }
    auto value() -> T& { return m_value; }
    auto reset() -> void { m_set = false; }
    template<class... Args>
    auto emplace(Args&&... args) -> void
    { m_value = T(std::forward<Args>(args)...); m_set = true; }
protected:
    optional_storage_value(): m_set(false) {}
    template<class... Args>
    optional_storage_value(Args&&... args)
        : m_value(std::forward<Args>(args)...), m_set(true) { }
    auto assign(const T &t) -> void { m_value = t; m_set = true; }
    auto assign(T &&t) -> void { m_value = std::move(t); m_set = true; }
    auto assign(const optional_storage_value &other) -> void
    {
        if (other.m_set)
            assign(other.m_value);
        else
            reset();
    }
    auto assign(optional_storage_value &&other) -> void
    {
        if (other.m_set)
            assign(std::move(other.m_value));
        else
            reset();
    }
    auto swap(optional_storage_value &other) -> void
    {
        if (m_set || other.m_set) {
            std::swap(m_value, other.m_value);
            std::swap(m_set, other.m_set);
        }
    }
private:
    T m_value;
    char m_set;
};

template<class T>
struct optional_storage_pointer {
    auto has_value() const -> bool { return !!m; }
    auto value() const -> const T& { return *m; }
    auto value() -> T& { return *m; }
    auto reset() -> void { m.reset(); }
    template<class... Args>
    auto emplace(Args&&... args) -> void
    { m.reset(new T(std::forward<Args>(args)...)); }
protected:
    optional_storage_pointer() {}
    template<class... Args>
    optional_storage_pointer(Args&&... args)
        : m(new T(std::forward<Args>(args)...)) { }
    auto assign(const T &t) -> void
    { if (!m) { m.reset(new T(t)); } else { *m = t; } }
    auto assign(T &&t) -> void
    { if (!m) { m.reset(new T(std::move(t))); } else { *m = std::move(t); } }
    auto assign(const optional_storage_pointer &other) -> void
    {
        if (other.m)
            assign(*other.m);
        else
            reset();
    }
    auto assign(optional_storage_pointer &&other) -> void { m.swap(other.m); }
    auto swap(optional_storage_pointer &other) -> void { m.swap(other.m); }
private:
    std::unique_ptr<T> m;
};

using namespace b0::meta;

template<class T>
using option_storage = if_t<
    sizeof(T) < sizeof(uintptr_t)
        && std::is_trivially_destructible_v<T>
        && std::is_trivially_constructible_v<T>
        && std::is_trivially_copyable_v<T>,
    optional_storage_value<T>, optional_storage_pointer<T>>;
}

template<class T>
class optional : public detail::option_storage<T> {
    using base_t = detail::option_storage<T>;
    B0_USING_REQ_T;
    B0_USING_EQ_V;
public:
    constexpr optional() { }
    constexpr optional(nullopt_t) { }
    constexpr optional(optional &&other) { this->assign(std::move(other)); }
    constexpr optional(const optional &other) { this->assign(other); }
    template<class... Args>
    constexpr optional(in_place_t, Args&&... args)
        : base_t(std::forward<Args>(args)...) { }
    template<class U, B0_REQ(!eq_v<std::decay_t<U>, optional>)>
    constexpr optional(U &&u): base_t(std::forward<U>(u)) { }
    auto operator = (nullopt_t) -> optional& { this->reset(); return *this; }
    auto operator = (optional &&rhs) -> optional&
    { this->assign(std::move(rhs)); return *this; }
    auto operator = (const optional &rhs) -> optional&
    {
        if (this != &rhs)
            this->assign(rhs);
        return *this;
    }
////    template<class U = T>
////    auto operator = (U &&rhs) -> req_t<b0::meta::eq_v<std::decay_t<U>, T>, optional&>
////    {
////        std::cout << "asdasd " << rhs << std::endl;
////        assign(std::forward<U>(rhs));
////        return *this;
////    }
//    template<class U>
//    auto operator= (U &&rhs) -> req_t<
//    <
//      !is_same<typename decay<U>::type, optional<T&>>::value,
//      optional&
//    >::type
//    = delete;
    auto operator == (const optional &rhs) const -> bool
    {
        if (rhs.has_value())
            return this->has_value() && this->value() == rhs.value();
        return !this->has_value();
    }
    auto operator != (const optional &rhs) const -> bool { return !(*this == rhs); }
    constexpr explicit operator bool() const { return this->has_value(); }
    constexpr auto operator->() const -> const T* { return &this->value(); }
    auto operator->() -> T* { return &this->value(); }
    constexpr auto operator*() const& -> const T& { return this->value(); }
    constexpr auto operator*() const&& -> const T&& { return std::move(this->value()); }
    auto operator*() & -> T& { return this->value(); }
    auto operator*() && -> T&& { return std::move(this->value()); }
    template<class U>
    auto value_or(U &&def) const& -> T
    {
        if (this->has_value())
            return this->value();
        return static_cast<T>(std::forward<U>(def));
    }
    template<class U>
    auto value_or(U &&def) && -> T
    {
        if (this->has_value())
            return std::move(this->value());
        return static_cast<T>(std::forward<U>(def));
    }
    auto swap(optional &other) -> void { base_t::swap(other); }
};

template<class T>
inline auto make_optional(T &&t) -> optional<std::decay_t<T> >
{
    return optional<std::decay_t<T>>(in_place, std::forward<T>(t));
}

template<class T, class... Args>
inline auto make_optional(Args&&... args) -> optional<T>
{
    return optional<T>(in_place, std::forward<Args>(args)...);
}

}
