#pragma once

#include <memory>
#include "./meta/meta.hpp"
#include "./type_id.hpp"
#include "./utility.hpp"

namespace b0 {

namespace detail {

struct any_storage_base {
    virtual ~any_storage_base() = default;
    virtual auto info() const -> const std::type_info& = 0;
    virtual auto id() const -> type_id_t = 0;
    virtual auto clone() const -> any_storage_base* = 0;
    virtual auto assign(const any_storage_base &other) -> void = 0;
    virtual auto compare(const any_storage_base &) const -> bool { return false; }
};

template<class T>
struct any_storage : any_storage_base {
    template<class... Args>
    any_storage(Args&&... args): m(std::forward<Args>(args)...) { }
    auto info() const -> const std::type_info& override { return typeid(T); }
    auto id() const -> type_id_t override { return type_id<T>(); }
    auto clone() const -> any_storage_base* override { return new any_storage<T>(m); }
    auto value() const -> const T& { return m; }
    auto value() -> T& { return m; }
    auto assign(const any_storage_base &other) -> void override
    { m = static_cast<const any_storage&>(other).m; }
private:
    T m;
};

}

class any {
    template<class T>
    using storage = detail::any_storage<T>;
public:
    any() { }

    any(const any &other): m(other.clone()) { }

    any(any &&other): m(std::move(other.m)) { }

    template<class U, class T = std::decay_t<U>,
             B0_REQ(!b0::meta::eq_v<T, any>)>
    any(U &&other)
        : m(new storage<T>(std::forward<U>(other))) { }

    template<class T, class... Args>
    any(in_place_type_t<T>, Args&&... args)
        : m(new storage<T>>(std::forward<Args>(args)...)) { }

    auto type_id() const -> type_id_t { return m ? m->id() : b0::type_id<void>(); }

    auto operator = (const any &rhs) -> any&
    {
        if (this == &rhs)
            return *this;
        if (rhs.m) {
            if (m) {
                if (m->id() == rhs.m->id()) {
                    m->assign(*rhs.m);
                    return *this;
                }
            }
            m.reset(rhs.m->clone());
            return *this;
        }
        m.reset();
        return *this;
    }

    auto operator = (any &&rhs) -> any& { m.swap(rhs.m); return *this; }

    auto reset() -> void { m.reset(); }

    template<class T, class... Args>
    auto emplace(Args&&... args) -> void { m.reset(new storage<T>(std::forward<Args>(args)...)); }

    auto swap(any &other) -> void { m.swap(other.m); }

    auto has_value() const -> bool { return !!m; }

    template<class T>
    auto value() const -> const T& { return static_cast<const storage<T>*>(m.get())->value(); }

    template<class T>
    auto value() -> T& { return static_cast<storage<T>*>(m.get())->value(); }

    template<class U, class T = std::decay_t<U>>
    auto value_or(U &&u) const& -> T
    { return is<T>() ? value<T>() : std::forward<U>(u); }

    template<class U, class T = std::decay_t<U>>
    auto value_or(U &&u) && -> T
    { return is<T>() ? std::move(value<T>()) : std::forward<U>(u); }

    template<class T>
    auto is() const -> bool { return type_id() == b0::type_id<T>(); }

    auto type() const -> const std::type_info& { return m ? m->info() : typeid(void); }
private:
    auto clone() const -> detail::any_storage_base* { return m ? m->clone() : nullptr; }
    std::unique_ptr<detail::any_storage_base> m;
};

template<class T>
auto make_any(T &&t) -> any { return any(std::forward<T>(t)); }

template<class T, class... Args>
auto make_any(Args&&... args) -> any { return any(in_place_type<T>, std::forward<Args>(args)...); }

}
