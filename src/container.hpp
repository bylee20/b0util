#pragma once

#include "./meta/container.hpp"

namespace b0 {

template<class T>
constexpr inline auto try_reserve(T &t, meta::size_type_t<T> size)
-> meta::req_t<meta::is_reservable<T>::value> { t.reserve(size); }
template<class T>
constexpr inline auto try_reserve(T&, meta::size_type_t<T>)
-> meta::req_t<!meta::is_reservable<T>::value> { }

template<class T, class... Args>
constexpr inline auto emplace_or_push_back(T &t, Args&&... args) -> meta::req_t<meta::can_emplace_back_v<T> >
{ t.emplace_back(std::forward<Args>(args)...); }

template<class T, class Value>
constexpr inline auto emplace_or_push_back(T &t, Value &&v) -> meta::req_t<!meta::can_emplace_back_v<T> >
{ t.push_back(std::forward<Value>(v)); }

template<class T, class Value, class... Args>
constexpr inline auto emplace_or_push_back(T &t, Value &&v, Args&&... args) -> meta::req_t<!meta::can_emplace_back_v<T> >
{ t.push_back(typename T::value_type(std::forward<Value>(v), std::forward<Args>(args)...)); }

template<class T>
inline auto make_container(meta::size_type_t<T> size) -> meta::req_t<meta::is_resizable<T>::value, T>
{
    T container;
    container.resize(size);
    return container;
}

template<class T>
inline auto make_container(meta::size_type_t<T> size) -> meta::req_t<!meta::is_resizable<T>::value, T>
{
    T container;
    try_reserve(container, size);
    for (int i = 0; i < size; ++i)
        emplace_or_push_back(container);
    return container;
}

template<class T>
inline auto make_container_reserved(meta::size_type_t<T> size) -> T
{
    T container;
    try_reserve(container, size);
    return container;
}

}
