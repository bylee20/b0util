#pragma once

#include "./meta.hpp"
#include <iterator>

namespace b0 {

namespace meta {

namespace detail {

template<class T, class = void>
struct is_iterator : false_ {};

template<class T>
struct is_iterator<T, b0_valid_expr(typename std::iterator_traits<T>::pointer())> : true_ {};

}

template<class T>
using is_iterator = detail::is_iterator<T>;

template<class T>
constexpr bool is_iterator_v = is_iterator<T>::value;

template<class T>
using iterator_category = typename std::iterator_traits<T>::iterator_category;

template<class T, class Tag>
using is_iterator_category = std::is_convertible<iterator_category<T>, Tag>;

template<class T>
using is_random_access_iterator = is_iterator_category<T, std::random_access_iterator_tag>;

template<class T>
constexpr bool is_random_access_iterator_v = is_random_access_iterator<T>::value;

}

}

