#pragma once

#include "./meta.hpp"

namespace b0 {namespace meta {

namespace detail {

B0_DECLARE_HAS_MEMBER(reserve);
B0_DECLARE_HAS_MEMBER(resize);

template<class T, class = void>
struct is_container_impl : false_ {};

template<class T>
struct is_container_impl<
    T, b0_valid_expr(
        typename T::value_type(),
        std::declval<T>().begin(),
        std::declval<T>().end(),
        std::declval<T>().size())
> : true_ {};

template<class T, class = void>
struct container_value_type {};

template<class T>
struct container_value_type<
    T, b0_valid_expr(typename T::value_type())
> : wrap_type<typename T::value_type> {};

template<class T, class = void>
struct can_emplace_back : false_ {};

template<class T>
struct can_emplace_back<T,
    b0_valid_expr(std::declval<T>().emplace_back(std::declval<typename T::value_type>()))
> : true_ {};

template<class T, class = void>
struct can_push_back : false_ {};

template<class T>
struct can_push_back<T,
    b0_valid_expr(std::declval<T>().push_back(std::declval<typename T::value_type>()))
> : true_ {};

template<class T, class = void>
struct is_range : b0::meta::false_ {};

template<class T>
struct is_range<T, decltype(valid_expr(
    std::begin(std::declval<T>()),
    std::end(std::declval<T>())))
> : b0::meta::true_ {};

template<class T, class = void>
struct has_size : false_ {};

template<class T>
struct has_size<T, decltype(valid_expr(std::size(std::declval<T>())))> : true_ {};

}

template<class T>
using has_size = detail::has_size<T>;

template<class T>
constexpr bool has_size_v = has_size<T>::value;

template<class T>
using is_reservable = bool_<detail::has_reserve<T>::value>;
template<class T>
constexpr bool is_reservable_v = is_reservable<T>::value;

template<class T>
using is_resizable = bool_<detail::has_resize<T>::value>;

template<class T>
constexpr bool is_resizable_v = is_resizable<T>::value;

template<class T>
using can_emplace_back = detail::can_emplace_back<T>;
template<class T>
constexpr bool can_emplace_back_v = can_emplace_back<T>::value;

template<class T>
using can_push_back = detail::can_push_back<T>;
template<class T>
constexpr bool can_push_back_v = can_push_back<T>::value;

template<class T>
using is_container = detail::is_container_impl<T>;
template<class T>
constexpr bool is_container_v = is_container<T>::value;

template<class T>
using is_range = detail::is_range<T>;

template<class T>
constexpr bool is_range_v = is_range<T>::value;

template<class T>
using is_range_container = bool_<is_range_v<T> && is_container_v<T> && can_push_back_v<T>>;

template<class T>
constexpr bool is_range_container_v = is_range_container<T>::value;

template<class T>
using size_type = wrap_type<decltype(std::size(std::declval<T>()))>;

template<class T>
using size_type_t = typename size_type<T>::type;

}}
