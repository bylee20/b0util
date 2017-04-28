#pragma once

#include <cinttypes>

namespace b0 {

struct type_id_t {
    type_id_t(): type_id_t(make_id<void>()) { }

    constexpr auto operator == (type_id_t other) const -> bool
    { return m_id == other.m_id; }

    constexpr auto operator != (type_id_t other) const -> bool
    { return m_id != other.m_id; }

    constexpr auto operator < (type_id_t other) const -> bool
    { return m_id < other.m_id; }

    constexpr auto operator > (type_id_t other) const -> bool
    { return m_id > other.m_id; }

    constexpr auto operator <= (type_id_t other) const -> bool
    { return m_id <= other.m_id; }

    constexpr auto operator >= (type_id_t other) const -> bool
    { return m_id >= other.m_id; }

    template<class T>
    static auto get() -> type_id_t
    { static const type_id_t type_id(make_id<T>()); return type_id; }
private:
    constexpr type_id_t(uintptr_t id): m_id(id) { }

    template<class T>
    static auto make_id() -> uintptr_t
    { static uint32_t dummy; return reinterpret_cast<uintptr_t>(&dummy); }

    template<class T>
    friend auto type_id() -> type_id_t;

    uintptr_t m_id;
};

template<class T>
inline auto type_id() -> type_id_t { return type_id_t::get<T>(); }

}
